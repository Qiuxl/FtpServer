#pragma once

#include<iostream>
#include<vector>
#include<winsock.h>

class FtpServer
{
public:
	FtpServer();
	~FtpServer();
	bool createsock();  // 加载socket库
	bool Onaccept();
	void managesocks();  // 核心调度函数，这个类的核心函数
	bool sendresponse(SOCKET &sock,std::string msg); //  发送相应字符串
	bool senddata(std::string msg);    // 发送数据
	void loadfile(SOCKET &sock,std::string filename);   // 接收文件
	void sendfile(SOCKET &sock,std::string filename, int length);
	void LIST_cmd(SOCKET ctrlsock, SOCKET &sock);  // 发送当前目录下的文件信息
	void CD_cmd(std::string &dir);   //  执行简单的记录当前目录的函数	
	bool CheckDirFileExist(std::string &DorF);  //用于用户命令要求进入的文件夹是否存在
	void dirtraverse(std::string dir);
	void GetLoaclIp();   // 获取本机ip地址
	int GetFileLength(const char *file_path);    // GET the size of file given 
protected:
 
	void cmd_analysis(std::string &cmd);      //
	bool checkuserinfo(std::string username, std::string password); // 检验用户是否合法，查看本地用户库里面是否有


private:
	//std::vector<std::thread> Th;
	int datamode;  //记录传输模式
	int index;
	SOCKET for_connect;
	char Localip[24];
	SOCKET controlsock;
	SOCKET datasock;
	char message[4096];
	char databuff[4096];
	char DirInfo[8192];
	std::string localpath;
	std::string CurrentDir; 
	std::string RootDir;  // 
	char cliendcmd[4096];
	std::string cmd_part1;
	std::string cmd_part2;
	unsigned int ip_part[4];
};

