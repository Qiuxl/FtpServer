#pragma once
#include<iostream>
#define USERNUM 2

static std::string command[] = {
	"FTP",
	"USER",
	"PASS",
	"CDUP",
	"CWD",
	"RETR",
	"STOR",
	"LIST"
};

class userinfo{
public:
	userinfo(std::string a = "anonymous", std::string b = "none");
	std::string username;
	std::string password;
};


