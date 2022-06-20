#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "Train.h"
#include "User.h"
#include "token_scanner.h"
#include "bpt.h"
#include "linked_hashmap.hpp"
#include "string.hpp"

//void Rollback(int time);//ʱ�����

enum Command{add_user,login,logout,query_profile,modify_profile,query_order,
    add_train,delete_train,release_train,query_train,query_ticket,query_transfer,
    buy_ticket,refund_ticket,
    rollback,clean,exit_};

const std::string Command_[]={"add_user","login","logout","query_profile","modify_profile","query_order",
                              "add_train","delete_train","release_train","query_train","query_ticket","query_transfer",
                              "buy_ticket","refund_ticket",
                              "rollback","clean","exit"};

Command find_Command(std::string s){
    for(int i=0;;i++)if(s==Command_[i])return (Command)i;
    return (Command)0;
}

int get_number(std::string str){
    if(str.size()==0)return -1;
    int ret=0;
    for(int i=0;i<(int)str.size();i++)
        if(isdigit(str[i]))ret=ret*10+str[i]-48;
    return ret;
}

std::string Str[130];
User_Management user;
TrainManagement train;
int timestamp;

void get_arr(int *A,std::string s){
    int sz=s.size(),k=0;
    for(int i=0,j;i<sz;i++){
        j=i;
        while(j<sz&&s[j]!='|')j++;
        A[k++]=get_number(s.substr(i,j-i));
        i=j;
    }
}

void get_arr(sjtu::string *A,std::string s){
    int sz=s.size(),k=0;
    for(int i=0,j;i<sz;i++){
        j=i;
        while(j<sz&&s[j]!='|')j++;
        A[k++]=sjtu::string(s.substr(i,j-i));
        i=j;
    }
}

void Add_train(){
    std::string *p=Str;
    static int prices[M],travelTimes[M],stopoverTimes[M];
    static sjtu::string stations[M];
    Date saleDateL(p['d'].substr(0,5)),saleDateR(p['d'].substr(6,5));

    get_arr(prices,p['p']);
    get_arr(travelTimes,p['t']);
    get_arr(stopoverTimes,p['o']);
    get_arr(stations,p['s']);

    train.add_train( p['i'],get_number(p['n']),stations,get_number(p['m']),
                     prices,travelTimes,stopoverTimes,(Date)p['x'],saleDateL,saleDateR,p['y'][0] );
}

void Buy_ticket(const std::string &username_,const std::string &trainID_,const Date &date,const int &num,const std::string &fr,const std::string &to,const std::string &flag){
    if(!user.Ask_Login(username_))return void(puts("-1"));
    train.buy_ticket(username_,trainID_,date,fr,to,num,flag.size()>0&&flag[0]=='t',timestamp);
}

void Refund_ticket(const std::string &username_,const std::string &kth){
    int k=1;
    if(kth.size()!=0)k=get_number(kth);
    if(!user.Ask_Login(username_))return void(puts("-1"));
    train.refund_ticket(H(username_),k);
}

void Query_order(const std::string &username_){
    if(!user.Ask_Login(username_))return void(puts("-1"));
    train.query_order(H(username_));
}

void Reset(){
    user.Reset();
    train.Reset();
}

void Process(){//��ȡָ�ִ��

    token_scanner str;
    std::string *p=Str;
//	int CT=0;
    while(1){
        bool flag=0,rd=str.read();
        timestamp=get_number(str.next_token());
        printf("[%d] ",timestamp);
        if (timestamp==30884) break;
        for(int i='a';i<'z';i++)Str[i]="";
        Command cmd=find_Command(str.next_token());
        std::string s=str.next_token(),c;
        while(s.size()>0){
            c=str.next_token();
            p[(int)s[1]]=c;
            s=str.next_token();
        }
//		CT++;
//		if(CT%1000==0){
//			std::cerr<<train.Size()+user.Size()<<std::endl;
//		}
        switch(cmd){
            case add_user: user.add_user(p['c'],p['u'],p['p'],p['n'],p['m'],get_number(p['g']));break;
            case login: user.login(p['u'],p['p']);break;
            case logout: user.logout(p['u']);break;
            case query_profile: user.query_profile(p['c'],p['u']);break;
            case modify_profile: user.modify_profile(p['c'],p['u'],p['p'],p['n'],p['m'],get_number(p['g']));break;

            case add_train: Add_train();break;
            case delete_train: train.delete_train(p['i']);break;
            case release_train: train.release_train(p['i'],timestamp);break;
            case query_train: train.query_train(p['i'],p['d']);break;
            case query_ticket: train.query_ticket(p['s'],p['t'],p['d'],p['p'].size()>0&&p['p'][0]=='c');break;
            case query_transfer: train.query_transfer(p['s'],p['t'],p['d'],p['p'].size()>0&&p['p'][0]=='c');break;

            case buy_ticket: Buy_ticket(p['u'],p['i'],p['d'],get_number(p['n']),p['f'],p['t'],p['q']);break;
            case refund_ticket: Refund_ticket(p['u'],p['n']);break;
            case query_order: Query_order(p['u']);break;

            case rollback: break;
            case clean: Reset();break;
            case exit_: flag=1;break;
        }
        if(flag){
            puts("bye");
            break;
        }
        if(!rd)break;

    }
}

int main(){
    //user.Print();
    //puts("----------");
    //train.Print();
    //freopen("2.in","r",stdin);
    //freopen("2.ans","w",stdout);
    Process();
    return 0;
}
