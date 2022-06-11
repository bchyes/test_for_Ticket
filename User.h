#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "utility.hpp"
#include "bpt.h" 
#include "string.hpp"

class User_Management;

class User{
private:
    char username[22];
    char password[32];
    char name[32];
    char mailAddr[32];
    int privilege;
	bool Login;
public:
	User();
	
	User(const std::string &username_,const std::string &password_,const std::string &name_,const std::string &mailAddr_,const int &privilege_);
	
	~User();
	
	void query_order();//query_order command 
	
	void refund_ticket(const int &place);//refund_ticket command 
	
	
	friend class User_Management;
};

class User_Management{
private:
public:
	sjtu::bpt<sjtu::string,User> pos;//UserID���û���ӳ�� 
	User_Management();
	
	~User_Management();
	
    void add_user(const std::string &cur_username,const std::string &username,const std::string &pwd,const std::string &name,
	 			  const std::string &mailAddr,const int &privilege);//add_user command 
    
    bool login(const std::string &username,const std::string &pwd);//login command 
    
    bool logout(const std::string &username,const bool &out=0);//logout command 
    
    void query_profile(const std::string &cur_username,const std::string &username);//query_profile command 
    
    void modify_profile(const std::string &cur_username,const std::string &username,const std::string &pwd,const std::string &name,
	 			  const std::string &mailAddr,const int &privilege);//modify_profile command 
	 			
	bool Ask_Login(const sjtu::string &username); 			
	
	void Reset();
};

User::User(){
	username[0]='\0';
	password[0]='\0';
	name[0]='\0';
	mailAddr[0]='\0';
	privilege=0;
	Login=0;
}

User::User(const std::string &username_,const std::string &password_,const std::string &name_,const std::string &mailAddr_,const int &privilege_){
	strcpy(username,username_.c_str());
	strcpy(password,password_.c_str());
	strcpy(name,name_.c_str());
	strcpy(mailAddr,mailAddr_.c_str());
	privilege=privilege_;
	Login=0; 
}

User::~User(){}

User_Management::User_Management():pos("file_user.dat","file_user_delete.dat"){}

User_Management::~User_Management(){}

void User_Management::add_user(const std::string &cur_username_,const std::string &username_,const std::string &pwd,const std::string &name,const std::string &mailAddr,const int &privilege){
	sjtu::string cur_username(cur_username_),username(username_);
	if(pos.empty()){
		pos.insert( sjtu::pair<sjtu::string,User>( username,User(username_,pwd,name,mailAddr,privilege) ) );
		puts("0");
		return;
	}
	if(!pos.count(cur_username)||pos.count(username))return void(puts("-1"));
	if(!pos.find(cur_username).Login||privilege>=pos.find(cur_username).privilege)return void(puts("-1"));
	pos.insert( sjtu::pair<sjtu::string,User>( username,User(username_,pwd,name,mailAddr,privilege) ) );
	puts("0");
}

bool User_Management::login(const std::string &username_,const std::string &pwd){
	sjtu::string username(username_);
	if(!pos.count(username)||pos.find(username).Login)return puts("-1"),0;
	std::string Pwd(pos.find(username).password);
	if(Pwd==pwd){
		User tmp=pos.find(username);
		tmp.Login=1;
		pos.modify(username,tmp);
		return puts("0"),1;
	}
	return puts("-1"),0;
}

bool User_Management::logout(const std::string &username_,const bool &out){
	sjtu::string username(username_);
	if(pos.count(username)&&pos.find(username).Login==1){
		User tmp=pos.find(username);
		tmp.Login=0;
		pos.modify(username,tmp);
		return out?233:puts("0"),1;
	}
	return out?233:puts("-1"),0;
}

void User_Management::query_profile(const std::string &cur_username_,const std::string &username_){
	sjtu::string cur_username(cur_username_),username(username_);
	if(!pos.count(cur_username)||!pos.count(username))return void(puts("-1"));
	User Cur=pos.find(cur_username),Ask=pos.find(username);
	if(Cur.Login==0||(username_!=cur_username_&&Cur.privilege<=Ask.privilege))return void(puts("-1"));
	std::cout<<Ask.username<<' '<<Ask.name<<' '<<Ask.mailAddr<<' '<<Ask.privilege<<std::endl;
}

void User_Management::modify_profile(const std::string &cur_username_,const std::string &username_,const std::string &pwd,const std::string &name,const std::string &mailAddr,const int &privilege){
	sjtu::string cur_username(cur_username_),username(username_);
	if(!pos.count(cur_username)||!pos.count(username))return void(puts("-1"));
	User Cur=pos.find(cur_username),Ask=pos.find(username);
	if(Cur.Login==0||(username_!=cur_username_&&Cur.privilege<=Ask.privilege)||privilege>=Cur.privilege)return void(puts("-1"));
	if(pwd.size()>0)strcpy(Ask.password,pwd.c_str());
	if(name.size()>0)strcpy(Ask.name,name.c_str());
	if(mailAddr.size()>0)strcpy(Ask.mailAddr,mailAddr.c_str());
	if(privilege!=-1)Ask.privilege=privilege;
	pos.modify(username,Ask);
	std::cout<<Ask.username<<' '<<Ask.name<<' '<<Ask.mailAddr<<' '<<Ask.privilege<<std::endl;
}

bool User_Management::Ask_Login(const sjtu::string &username){
	return pos.count(username)&&pos.find(username).Login;
}

void User_Management::Reset(){
	pos.clean();
}
