#pragma once

#include<iostream>
#include<vector>
#include<winsock.h>

class FtpServer
{
public:
	FtpServer();
	~FtpServer();
	bool createsock();  // ����socket��
	bool Onaccept();
	void managesocks();  // ���ĵ��Ⱥ����������ĺ��ĺ���
	bool sendresponse(SOCKET &sock,std::string msg); //  ������Ӧ�ַ���
	bool senddata(std::string msg);    // ��������
	void loadfile(SOCKET &sock,std::string filename);   // �����ļ�
	void sendfile(SOCKET &sock,std::string filename, int length);
	void LIST_cmd(SOCKET ctrlsock, SOCKET &sock);  // ���͵�ǰĿ¼�µ��ļ���Ϣ
	void CD_cmd(std::string &dir);   //  ִ�м򵥵ļ�¼��ǰĿ¼�ĺ���	
	bool CheckDirFileExist(std::string &DorF);  //�����û�����Ҫ�������ļ����Ƿ����
	void dirtraverse(std::string dir);
	void GetLoaclIp();   // ��ȡ����ip��ַ
	int GetFileLength(const char *file_path);    // GET the size of file given 
protected:
 
	void cmd_analysis(std::string &cmd);      //
	bool checkuserinfo(std::string username, std::string password); // �����û��Ƿ�Ϸ����鿴�����û��������Ƿ���


private:
	//std::vector<std::thread> Th;
	int datamode;  //��¼����ģʽ
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

