#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "utility.hpp"
#include "bpt.h" 
#include "string.hpp"
#include "vector.hpp"
#include "Date.h"

struct Ticket{
	sjtu::string trainID,From,To;
	Date TimeL,TimeR;
	int cost,time,num;
	Ticket():cost(0),time(0),num(2e9){}
	Ticket(const sjtu::string &res):trainID(res){}
	Ticket(const std::string &res):trainID(res){}
	Ticket(const Ticket &res):
	trainID(res.trainID),From(res.From),To(res.To),
	TimeL(res.TimeL),TimeR(res.TimeR),
	cost(res.cost),time(res.time),num(res.num){}
	void print()  const{
		trainID.print();putchar(' ');
		From.print();putchar(' ');
		TimeL.print();
		printf(" -> ");
		To.print();putchar(' ');
		TimeR.print();
		printf(" %d %d\n",cost,num);
	}
};

struct Transfer_Ticket{
	Ticket A,B;
	int time,cost;
	Transfer_Ticket(){};
	Transfer_Ticket(int x):time(x){}
	Transfer_Ticket(const Ticket &a,const Ticket &b,const int &t):A(a),B(b){
		time=t+A.time+B.time;
		cost=A.cost+B.cost;
	}
}; 

bool Cmp1(const Transfer_Ticket &a,const Transfer_Ticket &b){
	if(a.time!=b.time)return a.time<b.time;
	if(a.cost!=b.cost)return a.cost<b.cost;
	if(a.A.trainID!=b.A.trainID)return a.A.trainID<b.A.trainID;
	return a.B.trainID<b.B.trainID;
}

bool Cmp2(const Transfer_Ticket &a,const Transfer_Ticket &b){
	if(a.cost!=b.cost)return a.cost<b.cost;
	if(a.time!=b.time)return a.time<b.time;
	if(a.A.trainID!=b.A.trainID)return a.A.trainID<b.A.trainID;
	return a.B.trainID<b.B.trainID;
}

bool cmp1(const Ticket &a,const Ticket &b){
	return a.time<b.time||(a.time==b.time&&a.trainID<b.trainID);
}
bool cmp2(const Ticket &a,const Ticket &b){
	return a.cost<b.cost||(a.cost==b.cost&&a.trainID<b.trainID);
}

typedef sjtu::pair<sjtu::string,int> type1;
typedef sjtu::pair<sjtu::string,Ticket> type2;


class TicketManagement{
private:
	
	sjtu::bpt<type1,Ticket> get_ticket;//(username,kth) -> Ticket
	sjtu::bpt<type1,type2 > inqu_ticket;//(trainID,timestamp) -> (username,Ticket)
	sjtu::bpt<sjtu::string,int> num;//username -> tt
	sjtu::bpt<type1,int> pos_timestamp;//(username,kth) -> timestamp
	sjtu::bpt<int,int> pos_back;//timestamp ->kth
	

public:
	TicketManagement();
	
	~TicketManagement();
	
	void insert_ticket(const sjtu::string &username,const int &timestamp,const Ticket &t,const bool flag);
	
	void query_order(const sjtu::string &username);
	
	Ticket refund_ticket(const sjtu::string &username,const int &k);
	
	sjtu::vector<sjtu::pair<type1,type2> > all_ticket(const sjtu::string &username);
	
	void Update_ticket(const sjtu::string &username,const Ticket &t,const int &timestamp);
	
	void Reset();
	
};

TicketManagement::TicketManagement():get_ticket("file_get_ticket","file_get_ticket_delete"),
						   		inqu_ticket("file_inqu_ticket","file_inqu_ticket_delete"),
								num("file_num","file_num_delete"),
								pos_timestamp("file_pos_timestamp","file_pos_timestamp_delete"),
								pos_back("file_pos_back","file_pos_back_delete"){}
						   
TicketManagement::~TicketManagement(){};

void TicketManagement::insert_ticket(const sjtu::string &username,const int &timestamp,const Ticket &t,const bool flag){
	Ticket tmp=t;
	if(flag==0)tmp.time=1;
	else tmp.time=2;
	if(num.empty()||!num.count(username))num.insert(sjtu::pair<sjtu::string,int>(username,0));
	int tt=num.find(username);
	get_ticket.insert( sjtu::pair<type1,Ticket>(type1(username,++tt),tmp) );
	pos_timestamp.insert( sjtu::pair<type1,int>(type1(username,tt),timestamp) );
	pos_back.insert( sjtu::pair<int,int>(timestamp,tt) );
	num.modify(username,tt);
	if(tmp.time==2)inqu_ticket.insert( sjtu::pair<type1,type2>(type1(t.trainID,timestamp),type2(username,tmp)) );
}

void TicketManagement::query_order(const sjtu::string &username_){
	sjtu::string username(username_);
	const sjtu::vector<Ticket> &vec=get_ticket.traverse(type1(username,0),type1(username,2e9));
	printf("%d\n",(int)vec.size());
	for(int i=(int)vec.size()-1;i>=0;i--){
		if(vec[i].time==1)printf("[success] ");
		else if(vec[i].time==2)printf("[pending] ");
		else printf("[refunded] ");
		vec[i].print();
	}
}

Ticket TicketManagement::refund_ticket(const sjtu::string &username,const int &k){
	if(!num.count(username))return Ticket("");
	int tt=num.find(username);
	if(k>tt)return Ticket("");
	Ticket t=get_ticket.find(type1(username,tt-k+1));
	if(t.time==3)return Ticket("");
	int last_=t.time;
	t.time=3;
	get_ticket.modify( type1(username,tt-k+1) , t );
	if(last_==2)inqu_ticket.erase( type1(t.trainID,pos_timestamp.find(type1(username,tt-k+1)) ) );
	t.time=last_;
	return t;
}

sjtu::vector<sjtu::pair<type1,type2> > TicketManagement::all_ticket(const sjtu::string &trainID){
	if(inqu_ticket.empty())return sjtu::vector<sjtu::pair<type1,type2> >();
	return inqu_ticket.traverse_val(type1(trainID,0),type1(trainID,2e9));
}

void TicketManagement::Update_ticket(const sjtu::string &username,const Ticket &t,const int &timestamp){
	inqu_ticket.erase( type1(t.trainID,timestamp) );
	int bk=pos_back.find(timestamp);
	Ticket tmp = get_ticket.find( type1(username,bk) );
	tmp.time=1;
	get_ticket.modify(type1(username,bk),tmp);
}

void TicketManagement::Reset(){
	get_ticket.clean();
	inqu_ticket.clean();
}
