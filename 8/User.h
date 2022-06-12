#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "utility.hpp"
#include "bpt.h" 
#include "linked_hashmap.hpp"

class User_Management;

class User{
private:
    char username[22];
    char password[32];
    char name[22];
    char mailAddr[32];
    int privilege,Login;
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
	sjtu::bpt<size_t,User> pos;//UserID���û���ӳ�� 
	sjtu::linked_hashmap<size_t,bool>log;
public:
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

User_Management::User_Management():pos("file_user.dat","file_user_delete.dat"){
	if(pos.empty())return;
	const sjtu::vector< sjtu::pair<size_t,User> > &vec=pos.traverse_val(0,(size_t)-1);
	for(int i=0;i<vec.size();i++)
		log[vec[i].first]=vec[i].second.Login;
}

User_Management::~User_Management(){}

void User_Management::add_user(const std::string &cur_username_,const std::string &username_,const std::string &pwd,const std::string &name,const std::string &mailAddr,const int &privilege){
	size_t cur_username=H(cur_username_),username=H(username_);
	if(pos.empty()){
		pos.insert( sjtu::pair<size_t,User>( username,User(username_,pwd,name,mailAddr,privilege) ) );
		log[username]=0;
		puts("0");
		return;
	}
	
	if(!log.count(cur_username)||log.count(username))return void(puts("-1"));
	if(!log[cur_username]||privilege>=pos.find(cur_username).privilege)return void(puts("-1"));
	pos.insert( sjtu::pair<size_t,User>( username,User(username_,pwd,name,mailAddr,privilege) ) );
	log[username]=0;
	puts("0");
}

bool User_Management::login(const std::string &username_,const std::string &pwd){
	size_t username=H(username_);
	if(!log.count(username)||log[username]==1)return puts("-1"),0;
	User now=pos.find(username);
	std::string Pwd(now.password);
	if(Pwd==pwd){
		now.Login=1;
		pos.modify(username,now);
		log[username]=1;
		return puts("0"),1;
	}
	return puts("-1"),0;
}

bool User_Management::logout(const std::string &username_,const bool &out){
	size_t username=H(username_);
	if(log.count(username)&&log[username]==1){
		User tmp=pos.find(username);
		tmp.Login=0;
		pos.modify(username,tmp);
		log[username]=0;
		return out?233:puts("0"),1;
	}
	return out?233:puts("-1"),0;
}

void User_Management::query_profile(const std::string &cur_username_,const std::string &username_){
	size_t cur_username=H(cur_username_),username=H(username_);
	if(!log.count(cur_username)||!log.count(username))return void(puts("-1"));
	const User &Cur=pos.find(cur_username),&Ask=pos.find(username);
	if(Cur.Login==0||(username_!=cur_username_&&Cur.privilege<=Ask.privilege))return void(puts("-1"));
	std::cout<<Ask.username<<' '<<Ask.name<<' '<<Ask.mailAddr<<' '<<Ask.privilege<<std::endl;
}

void User_Management::modify_profile(const std::string &cur_username_,const std::string &username_,const std::string &pwd,const std::string &name,const std::string &mailAddr,const int &privilege){
	size_t cur_username=H(cur_username_),username=H(username_);
	if(!log.count(cur_username)||!log.count(username))return void(puts("-1"));
	const User &Cur=pos.find(cur_username);
	User Ask=pos.find(username);
	if(Cur.Login==0||(username_!=cur_username_&&Cur.privilege<=Ask.privilege)||privilege>=Cur.privilege)return void(puts("-1"));
	if(pwd.size()>0)strcpy(Ask.password,pwd.c_str());
	if(name.size()>0)strcpy(Ask.name,name.c_str());
	if(mailAddr.size()>0)strcpy(Ask.mailAddr,mailAddr.c_str());
	if(privilege!=-1)Ask.privilege=privilege;
	pos.modify(username,Ask);
	std::cout<<Ask.username<<' '<<Ask.name<<' '<<Ask.mailAddr<<' '<<Ask.privilege<<std::endl;
}

bool User_Management::Ask_Login(const sjtu::string &username_){
	size_t username=H(username_.change());
	return log.count(username)&&log[username];
}

void User_Management::Reset(){
	pos.clean();
}