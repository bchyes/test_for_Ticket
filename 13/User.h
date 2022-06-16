#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "utility.hpp"
#include "bpt.h" 
#include "linked_hashmap.hpp"

class User_Management;


struct User{
    char username[22];
    char password[32];
    char name[22];
    char mailAddr[32];
    int privilege;
	User();
	
	User(const std::string &username_,const std::string &password_,const std::string &name_,const std::string &mailAddr_,const int &privilege_);
	
	~User();
	
};

class User_Management{
private:
	sjtu::bpt<size_t,User,254,32,2> pos;//UserID���û���ӳ��
	sjtu::linked_hashmap<size_t,bool>log;
public:
	User_Management();

	~User_Management();
	
    void add_user(const std::string &cur_username,const std::string &username,const std::string &pwd,const std::string &name,
	 			  const std::string &mailAddr,const int &privilege);//add_user command 
    
    void login(const std::string &username,const std::string &pwd);//login command 
    
    void logout(const std::string &username);//logout command 
    
    void query_profile(const std::string &cur_username,const std::string &username);//query_profile command 
    
    void modify_profile(const std::string &cur_username,const std::string &username,const std::string &pwd,const std::string &name,
	 			  const std::string &mailAddr,const int &privilege);//modify_profile command 
	 			
	bool Ask_Login(const std::string &username); 			
	
	void Reset();

	void Print();

	void Rollback(const int &time);
};
void User_Management::Print(){
	pos.print();
}

User::User(){
	username[0]='\0';
	password[0]='\0';
	name[0]='\0';
	mailAddr[0]='\0';
	privilege=0;
}

User::User(const std::string &username_,const std::string &password_,const std::string &name_,const std::string &mailAddr_,const int &privilege_){
	strcpy(username,username_.c_str());
	strcpy(password,password_.c_str());
	strcpy(name,name_.c_str());
	strcpy(mailAddr,mailAddr_.c_str());
	privilege=privilege_;
}

User::~User(){}

User_Management::User_Management():pos("file_user.dat","file_user2.dat","file_user_delete.dat","file_user2_delete.dat"){}

User_Management::~User_Management(){}

void User_Management::add_user(const std::string &cur_username_,const std::string &username_,const std::string &pwd,const std::string &name,const std::string &mailAddr,const int &privilege){
	size_t cur_username=H(cur_username_),username=H(username_);
	if(pos.empty()){
		pos.insert( sjtu::pair<size_t,User>( username,User(username_,pwd,name,mailAddr,privilege) ) );
		puts("0");
		return;
	}
	if(!log.count(cur_username)||pos.count(username))return void(puts("-1"));
	if(privilege>=pos.find(cur_username).privilege)return void(puts("-1"));
	pos.insert( sjtu::pair<size_t,User>( username,User(username_,pwd,name,mailAddr,privilege) ) );
	puts("0");
}

void User_Management::login(const std::string &username_,const std::string &pwd){
	size_t username=H(username_);
	if(log.count(username))return void(puts("-1"));
	auto now=pos.find2(username);
	if(now.first==0)return void(puts("-1"));
	const std::string &Pwd(now.second.password);
	if(Pwd==pwd){
		log[username]=1;
		puts("0");
	}else puts("-1");
}

void User_Management::logout(const std::string &username_){
	size_t username=H(username_);
	if(log.count(username)){
		log.erase(log.find(username));
		puts("0");
	}else puts("-1");
}

void User_Management::query_profile(const std::string &cur_username_,const std::string &username_){
	size_t cur_username=H(cur_username_),username=H(username_);
	if(!log.count(cur_username))return void(puts("-1"));
	const User &Cur=pos.find(cur_username);
	auto tmp=pos.find2(username);
	if(tmp.first==0)return void(puts("-1"));
	const User &Ask=tmp.second;
	if((username_!=cur_username_&&Cur.privilege<=Ask.privilege))return void(puts("-1"));
	std::cout<<Ask.username<<' '<<Ask.name<<' '<<Ask.mailAddr<<' '<<Ask.privilege<<std::endl;
}

void User_Management::modify_profile(const std::string &cur_username_,const std::string &username_,const std::string &pwd,const std::string &name,const std::string &mailAddr,const int &privilege){
	size_t cur_username=H(cur_username_),username=H(username_);
	if(!log.count(cur_username))return void(puts("-1"));
	const User &Cur=pos.find(cur_username);
	auto tmp=pos.find2(username);
	if(tmp.first==0)return void(puts("-1"));
	User Ask=tmp.second;
	if((username_!=cur_username_&&Cur.privilege<=Ask.privilege)||privilege>=Cur.privilege)return void(puts("-1"));
	if(pwd.size()>0)strcpy(Ask.password,pwd.c_str());
	if(name.size()>0)strcpy(Ask.name,name.c_str());
	if(mailAddr.size()>0)strcpy(Ask.mailAddr,mailAddr.c_str());
	if(privilege!=-1)Ask.privilege=privilege;
	pos.modify(username,Ask);
	std::cout<<Ask.username<<' '<<Ask.name<<' '<<Ask.mailAddr<<' '<<Ask.privilege<<std::endl;
}

bool User_Management::Ask_Login(const std::string &username_){
	return log.count(H(username_));
}

void User_Management::Reset(){
	pos.clean();
}
