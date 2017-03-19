#include"resourse.h"
#include"Server.h"

#include<string>
using namespace std;

FtpServer *server = new FtpServer();
int main()
{
	try{
		server->createsock();
		server->dirtraverse(string("./"));
		server->GetLoaclIp();
		cout << server->CheckDirFileExist(string("TEST.cpp")) << endl;
		server->managesocks();
	}
	catch (string &e)
	{
		cout << e << endl;
	}
	return 0;
}