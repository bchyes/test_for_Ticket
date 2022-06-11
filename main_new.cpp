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

sjtu::linked_hashmap<std::string,bool>online;
//sjtu::bpt<int,sjtu::string> file_command("file_command","file_command_delete");

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
TicketManagement ticket;
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
    sjtu::string username(username_);
    if(!user.Ask_Login(username))return void(puts("-1"));
    Ticket2 tx=train.get_ticket(trainID_,date,fr,to,num);
    if(tx.cost==-1)return void(puts("-1"));
    Ticket t(tx,fr,to);
    if(t.num<num){
        t.num=num;
        if(flag.size()>0&&flag[0]=='t'){
            ticket.insert_ticket(username,timestamp,t,1);
            puts("queue");
        }else puts("-1");
        return;
    }
    t.num=num;
    train.Update_ticket(t);
    ticket.insert_ticket(username,timestamp,t,0);
    printf("%lld\n",1LL*num*t.cost);
}

void Refund_ticket(const std::string &username_,const std::string &kth){
    int k=1;
    if(kth.size()!=0)k=get_number(kth);
    sjtu::string username(username_);
    if(!user.Ask_Login(username))return void(puts("-1"));
    Ticket t=ticket.refund_ticket(username,k);
    if(t.trainID.size()==0)return void(puts("-1"));
    if(t.time==1){
        t.num=-t.num;
        train.Update_ticket(t);
    }
    const sjtu::vector<sjtu::pair<type1,type2> > &T=ticket.all_ticket(t.trainID);
    for(int i=0;i<(int)T.size();i++){
        Ticket nd = T[i].second.second;
        //std::cerr<<"OK"<<std::endl;
        if(train.query_ticket(t.trainID,nd.TimeL,nd.From,nd.To)>=nd.num){
            train.Update_ticket(nd);
            ticket.Update_ticket(T[i].second.first,nd,T[i].first.second);
        }
    }
    puts("0");
}

void Query_order(const std::string &username_){
    sjtu::string username(username_);
    if(!user.Ask_Login(username))return void(puts("-1"));
    ticket.query_order(username);
}

void Reset(){
    user.Reset();
    train.Reset();
    ticket.Reset();
}

void Process(){//��ȡָ�ִ��

    token_scanner str;
    std::string *p=Str;
    //int CT=0;
    while(1){
        bool flag=0,rd=str.read();
        timestamp=get_number(str.next_token());
        printf("[%d] ",timestamp);
        for(int i='a';i<'z';i++)Str[i]="";
        Command cmd=find_Command(str.next_token());
//		file_command.insert(sjtu::pair<int,std::string>( get_number(str.next_token()) , str.all_token() ) );
        std::string s=str.next_token(),c;
        while(s.size()>0){
            c=str.next_token();
            p[(int)s[1]]=c;
            s=str.next_token();
        }
        //CT++;
        //if(CT%1000==0){
        //		std::cerr<<CT/1000<<std::endl;
        //	}
        switch(cmd){
            case add_user: user.add_user(p['c'],p['u'],p['p'],p['n'],p['m'],get_number(p['g']));break;
            case login: user.login(p['u'],p['p'])?online[p['u']]=1:233;break;
            case logout: user.logout(p['u'])?online.erase(online.find(p['u'])),1:233;break;
            case query_profile: user.query_profile(p['c'],p['u']);break;
            case modify_profile: user.modify_profile(p['c'],p['u'],p['p'],p['n'],p['m'],get_number(p['g']));break;

            case add_train: Add_train();break;
            case delete_train: train.delete_train(p['i']);break;
            case release_train: train.release_train(p['i'],timestamp);break;
                //case query_train: train.query_train(p['i'],p['d']);break;
                //case query_ticket: train.query_ticket(p['s'],p['t'],p['d'],p['p'].size()>0&&p['p'][0]=='c');break;
                //case query_transfer: train.query_transfer(p['s'],p['t'],p['d'],p['p'].size()>0&&p['p'][0]=='c');break;

            case buy_ticket: Buy_ticket(p['u'],p['i'],p['d'],get_number(p['n']),p['f'],p['t'],p['q']);break;
                //case refund_ticket: Refund_ticket(p['u'],p['n']);break;
                //case query_order: Query_order(p['u']);break;

            case rollback: break;
            case clean: Reset();break;
            case exit_: flag=1;break;
        }
        if(flag){
            for(sjtu::linked_hashmap<std::string,bool>::iterator it=online.begin();it!=online.end();it++)
                user.logout(it->first,1);
            puts("bye");
            break;
        }
        if(!rd)break;
    }
}

int main(){
    //freopen("1.in","r",stdin);
    //freopen("1.ans","w",stdout);
    Process();
    return 0;
}
