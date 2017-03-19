#include"Server.h"
#include"resourse.h" 
#include<thread>
#include<direct.h>
#include<Windows.h>
#include<io.h>
#include<fstream>

// 由于c++ boost库里面的bind函数与socket函数冲突，
#include<string>
//using namespace std;
using std::cout;
using std::string;
using std::endl;
using std::ios;

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"msvcrtd.lib")

#define NAMENEED 0
#define PASSNEED 1
#define LOGINSUCESS 2
#define DATAPORT 2000
#define CONTROLPORT 21
#define BINARY 0
#define ASCII 1
extern userinfo users[];

struct sockaddr_in ctrl;
struct sockaddr_in data;
/*

*/
FtpServer::FtpServer()
{
	RootDir = ".";
	CurrentDir = RootDir;
	//std::bind(&FtpServer::Onaccept, this);
}

FtpServer::~FtpServer()
{

}

// all these are under passive mode

bool FtpServer::createsock()
{

	std::string error;

	WORD wVersionRequested;  // socket 版本，高字节是主版本，低字节是副版本
	WSADATA wsaData;
	struct sockaddr_in local;

	int err, result;
	wVersionRequested = MAKEWORD(2, 2);//将2和2凑成一个16位的word形变量，vc6.0认为word占了两个字节
	err = WSAStartup(wVersionRequested, &wsaData);  //加载socket库
	if (err != 0)
	{
		return false;
	}
	//WSACleanup();  //s释放的方法可以用这个
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return false;
	}

	controlsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// tcp，可靠的，不是udp
	datasock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

	if (controlsock == INVALID_SOCKET || datasock == INVALID_SOCKET)
	{
		error = "create socket failed!";
		throw error;
	}
	//WSAAsyncSelect(controlsock,)  // "127.0.0.1"
	ctrl.sin_family = AF_INET;//AF_INET针对internet,另一种形式AF_UNIX只能够用于单一的Unix系统进程通信（主机通信协议）
	ctrl.sin_addr.s_addr = inet_addr("127.0.0.1");//表示可以和任何主机通信
	ctrl.sin_port = htons(CONTROLPORT);//htons将短整形数据转换位网络字节顺序，数据端口2000

	data.sin_family = AF_INET;//AF_INET针对internet
	data.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//请记得将此地址改为能够通信的本机网卡ip地址
	data.sin_port = htons(DATAPORT);//htons将短整形数据转换位网络字节顺序
	int temp1;
	int temp2;
	temp1 = bind(controlsock, (struct sockaddr *)&ctrl, sizeof(ctrl)); 
	temp2 = bind(datasock, (struct sockaddr *)&data, sizeof(data));
	if (temp1== SOCKET_ERROR ||temp2 == SOCKET_ERROR)//将本地端口与socket返回的文件描述符绑定在一起
	{
		//int errcode = WSAGetLastError();
		printf("listenfailedwitherror:%ld\n", WSAGetLastError());
		error = "could not bind the socket with local port";
		throw(error);
	}
	int len1 = sizeof(ctrl);
	int len2 = sizeof(data);

	if (getsockname(controlsock, (sockaddr *)&ctrl, &len1) < 0
		|| getsockname(datasock, (sockaddr *)&data, &len2) < 0)
	{
		error = "get sock name failed";
		throw(error);
	}
//	printf("Socket port # %d  %d\n", ntohs(ctrl.sin_port), ntohs(data.sin_port));
	if (listen(controlsock, 2) == SOCKET_ERROR || listen(datasock, 2) == SOCKET_ERROR)
	{
		printf("listenfailedwitherror:%ld\n", WSAGetLastError());
		closesocket(controlsock);
		closesocket(datasock);
		WSACleanup();
		return 1;
	}
  //监听，允许最大监听数位2，将绑定的套接字变成监听套接字
	return true;
}


void FtpServer::managesocks()
{
	index = 0;
//	SOCKET TEMP;
	int zero = sizeof(ctrl);
	sockaddr_in sa_o;
	do{
		for_connect = accept(controlsock, (sockaddr *)&ctrl, &zero);
	//	TEMP = totalsock[index];
		if (for_connect == INVALID_SOCKET)
		{
			cout << WSAGetLastError() << endl;
			std::cout << "invalid socket" << std::endl;
			Sleep(1000);
		}
		else
		{
			cout << "there comes a connect request" << endl;
			Onaccept();   // 需要将函数声明为static成员
		//	std::thread(Onaccept);   多线程相应这一块还没搞定
		//	std::thread(FtpServer::Onaccept);
		//	std::thread t1([&] { this->Onaccept(); });
		//   std::thread t1(FtpServer::Onaccept());
		//	std::thread t1(this->Onaccept, this, &TEMP);
		}
	} while (1);
}

bool FtpServer::senddata(std::string msg)
{
	return true;
}
/*
函数：作为接收到socket连接请求的响应函数，核心函数
*/
bool FtpServer::Onaccept()
{
	SOCKET sock = for_connect;
	int num;
	int loginstatus = NAMENEED;  //用于记录登录的时候的状态
	string username;
	string password;
	SOCKET datatran;
	cout << sock << endl;
loop:
	memset(cliendcmd, 0, 4096);
	num = recv(sock, cliendcmd, sizeof(cliendcmd), 0);
	if (num < 0)
	{
		cout << "reading command failed " << endl;
	}
	if (num == 0)
	{
		closesocket(sock);
		cout << sock << " is closed " << endl;
		return true;
	}
	cmd_analysis(string(cliendcmd));
	if (cmd_part2 == "")    //只有一个指令的命令
	{
		if (cmd_part1 == "FTP" || cmd_part1 == "ftp")
		{
			sendresponse(sock, std::string("220 FTP service OK."));
		}
		else if (cmd_part1 == "QUIT")
		{
			closesocket(sock);
			cout << sock << "is closed " << endl;
			return true;
		}
		else if (cmd_part1 == "CDUP")
		{
			try{
				CD_cmd(std::string(".."));
				sendresponse(sock, std::string("250 Directory change to") + CurrentDir);
			}
			catch (string &e)
			{
				sendresponse(sock, std::string("500 it is the root directory"));
			}
		}
		else if (cmd_part1 == "LIST")
		{
			if (loginstatus == LOGINSUCESS)
				LIST_cmd(sock, datatran);
			else
			{
				sendresponse(sock, std::string("550 User not logged in."));
			}
		}
		else if (cmd_part1 == "PASV")   //进入被动模式
		{
			if (loginstatus == LOGINSUCESS)
			{
				try{
					GetLoaclIp();
				}
				catch (string &e)
				{
					cout << e << endl;
				}
				int port1 = DATAPORT / 256;
				int port2 = DATAPORT % 256;
				char tempmsg[256];
				memset(tempmsg, 0, sizeof(tempmsg));
				sprintf(tempmsg, "227 Entering Passive Mode (%d, %d, %d, %d, %d, %d)", ip_part[0], ip_part[1], ip_part[2], ip_part[3], port1, port2);
				sendresponse(sock, std::string(tempmsg));
				//		sendresponse(sock, std::string(tempmsg));
				//		sendresponse(sock, std::string("227 Entering Passive Mode (169, 254, 50, 195, 7, 208)"));
				//		sendresponse(sock, std::string("227 Entering Passive Mode (169, 254, 50, 195, 7, 208)"));
				cout << tempmsg << endl;
				cout << sock << endl;
				// 等待数据通道的连接
				int len = sizeof(data);
				datatran = accept(datasock, (sockaddr *)&data, &len);
				if (datatran == INVALID_SOCKET)
				{
					sendresponse(sock, std::string("550 Data transfer closed!."));
				}
			}
			else
			{
				sendresponse(sock, std::string("550 User not logged in."));
			}
		}
		else
		{
			sendresponse(sock, string("500 Command not understood"));
		}
	}
	else
	{
		if (cmd_part1 == "USER")
		{
			if (loginstatus == NAMENEED)
			{
				username = cmd_part2;
				sendresponse(sock, string("331 User name okay, need password."));
				loginstatus = PASSNEED;
			}
			else
			{
				sendresponse(sock, string("530 User name given,please send password."));
			}
		}
		else if (cmd_part1 == "PASS")
		{
			password = cmd_part2;
			if (checkuserinfo(username, password))
			{
				loginstatus = LOGINSUCESS;
				sendresponse(sock, string("230 User logged in."));
			}
		}
		else if (cmd_part1 == "CWD")
		{
			if (loginstatus == LOGINSUCESS)
			{
				CD_cmd(cmd_part2);
				sendresponse(sock, std::string("250 Directory change to") + CurrentDir);
			}
			else
			{
				sendresponse(sock, string("530 User not logged in."));
			}
		}
		else if (cmd_part1 == "RETR")
		{
			if (loginstatus == LOGINSUCESS)
			{
				if (!CheckDirFileExist(cmd_part2))
				{//文件不存在
					sendresponse(sock, string("500 File not exist!"));
				}
				else
				{
					int length = GetFileLength(cmd_part2.c_str());
					if (length == -1)
					{
						sendresponse(sock, string("500 Could not download directory"));
					}
					else
					{
						if (datatran == INVALID_SOCKET)
						{
							sendresponse(sock, std::string("550 Data transfer cloesed"));
							//return;
						}
						else
						{
							//150 Opening ASCII mode data connection
							sendresponse(sock, std::string("150 Opening ASCII mode data connection"));
							sendfile(datatran, cmd_part2, length);
						}
					}
				}
			}
			else
			{
				sendresponse(sock, string("530 User not logged in."));
			}
		}
		else if (cmd_part1 == "STOR")
		{
			if (loginstatus == LOGINSUCESS)
			{
				if (CheckDirFileExist(cmd_part2))   // 本地有同名文件不允许覆盖
				{
					sendresponse(sock, string("550 file exist already"));
				}
				else
				{
					if (datatran == INVALID_SOCKET)
					{
						sendresponse(sock, string("550 Data connection closed! Transfer failed"));
						//		226 Transfer complete.
					}
					else
					{
						//	125 Data connection already open; Transfer starting.
						sendresponse(sock, string("125 Data connection already open; Transfer starting."));
						loadfile(datatran, cmd_part2);
						sendresponse(sock, string("125 Transfer completes."));
					}
				}
			}
			else
			{
				sendresponse(sock, std::string("530 User not logged in."));
			}
		}
		else if (cmd_part1 == "TYPE")
		{
			//200 Type set to I.
			if (loginstatus == LOGINSUCESS)
			{
				if (cmd_part2 == "I")
				{
					datamode = BINARY;
					sendresponse(sock, string("200 Type set to I."));
				}
				else if (cmd_part2 == "A")
				{
					datamode = ASCII;
					sendresponse(sock, string("200 Type set to A"));
				}
				else{
					sendresponse(sock, string("500 unknown mode"));
				}
			}
			else
			{
				sendresponse(sock, std::string("530 User not logged in."));
			}
		}
	}
	datamode = ASCII;  // 默认将数据模式设置为阿斯克码
	goto loop;
}

/*
	function: according to the command from the client,send resposes
*/
bool FtpServer::sendresponse(SOCKET &sock,std::string msg)
{
	int num;
	msg += "\r\n";
	memset(message, 0, 4096);
	msg.copy(message, msg.length());
	num = send(sock, message, strlen(message), 0);
	if (num != msg.length())
		return false;
	return true;
}

/*
 */
void FtpServer::cmd_analysis(std::string &cmd)
{
	int index1, index2;
	index1 = cmd.find(' ');
	//空格即是隔开的地方
	if (index1 == std::string::npos)
	{
		index2 = cmd.find('\r');
		cmd_part1.assign(cmd, 0, index2);
		cmd_part2 = "";
		return;
	}
	cmd_part1.assign(cmd, 0, index1);
	index2 = cmd.find('\r');
	cmd_part2.assign(cmd, index1 + 1, index2 - index1 - 1);
}
/*
*/
// when the RootDir is "."
//the currentdir is "."
void FtpServer::CD_cmd(std::string &dir)
{
	string error;
	if (dir == "..")
	{
		if (CurrentDir == RootDir)
		{
			error = "the current directory is root,you can't back to upper";
			throw  error;
		}
		else
		{
			int index = CurrentDir.rfind('/');
			//更新当前目录
			CurrentDir.erase(index, CurrentDir.length() - index);
		}
	}
	else{
		if (!CheckDirFileExist(dir))
		{
			error = "Directory not exists!";
			throw(error);
		}
		else{
			//更新当前目录
			CurrentDir += "/" + cmd_part2;
		}
	}
}
/*

*/
void FtpServer::LIST_cmd(SOCKET ctrlsock,SOCKET &sock)
{
	sendresponse(ctrlsock, std::string("150 Opening ASCII mode data connection for /bin/ls."));
	if (sock == INVALID_SOCKET)
	{
		sendresponse(sock, std::string("550 Data transfer cloesed"));
		return;
	}
	dirtraverse(std::string(CurrentDir + "/"));   //遍历当前文件夹下的文件信息
//发送数据，假设目录信息不超过8096个字节
	send(sock, DirInfo, strlen(DirInfo), 0);
	memset(DirInfo, 0, sizeof(DirInfo));
	closesocket(sock);
	return;
}
/*
 
*/
bool FtpServer::checkuserinfo(std::string username, std::string password)
{
	int i;
	for (i = 0; i < USERNUM; i++)
	{
		if (users[i].username == username)
			break;
	}
	if (i == USERNUM)
		return false;
	if (users[i].password == password)
		return true;
	return false;
}
/*
遍历整个文件夹下面的文件，获取文件大小信息和最后的修改时间
written by Qiu Zenghui
finish time: 2016-10-08

*/
/*
使用windows 的api遍历本地文件夹下的目录及文件，需要处理unicode 和  ASCII 之间的编码转换问题
较为麻烦，将工程属性中的字符集设置为 "未设置"
*/
void FtpServer::dirtraverse(std::string dir)
{
	memset(DirInfo, 0, sizeof(DirInfo));
	WIN32_FIND_DATA fd;
	HANDLE hfile;
	char path[256];
	memset(path, 0, 256);
	dir.copy(path, dir.length());
	bool bret = true;
	strcat(path, "\*");
//	LPCSTR ptr = (const char *)path;

//	int n = MultiByteToWideChar(CP_ACP, 0, ptr, -1, NULL, 0);

//	LPCWSTR *b = new LPCWSTR[n];
//	MultiByteToWideChar(CP_ACP, 0, ptr, -1, (LPWSTR)b, n);

	hfile = FindFirstFile(path,&fd);
	char name[256];
	char time[64] = { 0 };
	char final[512];
	LARGE_INTEGER filesize;
	long long size;
	// 不使用Unicode字符集
	while (hfile != INVALID_HANDLE_VALUE&&bret)
	{
		memset(name, 0, sizeof(name));
		memset(time, 0, sizeof(time));
		memset(final, 0, sizeof(final));
		SYSTEMTIME st = { 0 };

		FileTimeToSystemTime(&fd.ftCreationTime, &st);
		sprintf(time, "%4d-%02d-%02d %02d:%02d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		if (strncmp(fd.cFileName, ".", 1) != 0 && strncmp(fd.cFileName, "..", 2)!=0)
		{
			strcpy(name, fd.cFileName);
			//strcpy(name, &fd.ftLastWriteTime);
			//cout << fd.ftLastWriteTime << endl;
		//	cout << buf << endl;
			if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{//如果是文件夹，直接打印出名字+<dir>+最后修改时间即可
				
				sprintf(final, "%-20s %-10s %-20s\r\n", name, "<DIR>", time);
			//	cout << final;
			}
			else 
			{
				filesize.HighPart = fd.nFileSizeHigh;
				filesize.LowPart = fd.nFileSizeLow;
				size = filesize.QuadPart;
				sprintf(final, "%-20s %-10lld %-20s\r\n", name, size, time);
			//	cout << final;
			}

		}
		bret = FindNextFile(hfile, &fd);
		strcat(DirInfo, final);
	}
//	cout << DirInfo << endl;
}

bool FtpServer::CheckDirFileExist(std::string &DorF)
{
	string temp = CurrentDir + "/" + DorF;
	if (_access(temp.c_str(), 0 )== 0)
	{
		return true;
	}
	return false;
}
/*
function: to get the local ip address

*/
void FtpServer::GetLoaclIp()
{
	//2.获取主机名  
	char hostname[256];
	string error;
	int ret = gethostname(hostname, sizeof(hostname));
	if (ret == SOCKET_ERROR)
	{
		error = "Unable to open the socket!";
		throw(error);
	}
	//3.获取主机ip  
	HOSTENT* host = gethostbyname(hostname);
	if (host == NULL)
	{
		error = "unable to get the local ip";
		throw(error);
	}
	//4.转化为char*并拷贝返回  
	strcpy(Localip, inet_ntoa(*(in_addr*)*host->h_addr_list));
	int i, j;
	int index = 0;
	int count;
	for (i = 0; i < strlen(Localip); i++)
	{
		count = 0;
		ip_part[index] = 0;
		for (j = i; j < strlen(Localip); j++)
		{
			if (Localip[j] >= '0'&&Localip[j] <= '9'&&Localip[j] != '\0')
			{
				ip_part[index] = ip_part[index]  * 10 + Localip[j] - '0';
				count++;
			}
			else
			{
				i += count;
				break;
			}
		}
		index++;
	}
}

int FtpServer::GetFileLength(const char *file_path)
{
	if (file_path == NULL)
	{
		return -1;
	}
	std::fstream file_in(file_path, std::ios::in | std::ios::binary);
	if (!file_in.is_open())
		return -1;
	file_in.seekg(0, ios::end);
	int file_length = file_in.tellg();  //返回读指针当前的位置
	file_in.close();
	//file_buffer_[file_length] = '\0';
	//	cout << file_buffer_ << endl;
	return file_length;
}
/*
function: send file
*/
void FtpServer::sendfile(SOCKET &sock, std::string filename, int length)
{
	memset(databuff, 0, sizeof(databuff));
	string filepath = CurrentDir + "/" + filename;
	std::fstream fin(filepath, ios::in | ios::binary);
	int bytecount = 0;
	while (bytecount < length)
	{
		fin.seekg(bytecount, ios::beg);
		if (bytecount + 4096 <= length)
		{
			fin.read(databuff, 4096);
			Sleep(10);
			send(sock, databuff, 4096, 0);
		}
		else
		{
			fin.read(databuff, length - bytecount);
			Sleep(10);
			send(sock, databuff, length - bytecount, 0);
		}
		bytecount += 4096;
	}
	closesocket(sock);
}
/*
	load filedata and write to the local file
*/
void FtpServer::loadfile(SOCKET &sock,std::string filename)
{
	string filepath = CurrentDir + "/" + filename;
	std::fstream fout(filepath, ios::out | ios::binary | ios::app);
	int length;
	while ((length = recv(sock, databuff, sizeof(databuff), 0)) > 0)
	{
		fout.write(databuff, length);
	}
}