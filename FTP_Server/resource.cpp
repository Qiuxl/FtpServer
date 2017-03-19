#include"resourse.h"
#include<Windows.h>
userinfo::userinfo(std::string a, std::string b)
{
	username = a;
	password = b;
}
userinfo a = userinfo();
userinfo b = userinfo(std::string("Qiu"), std::string("xiaoliang"));

userinfo users[] = {
	a,b
};
