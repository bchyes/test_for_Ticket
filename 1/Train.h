#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "string.hpp"
#include "vector.hpp"
#include "Ticket.h"

const int M=105;
typedef sjtu::pair<sjtu::string,sjtu::string> type3;

class TrainManagement;

class train{
private:
	sjtu::string trainID;
	int stationNum;
	sjtu::string stations[M];
	int prices[M],travelTimes[M],stopoverTimes[M];
	Date startTime,saleDateL,saleDateR;//saleDate_L是售票区间的左端点，saleDate_R是售票区间的右端点 
	char Type; 
	int rel; 
	
	int LeftTicket[M][M];//第i天发车的火车的第j张票还剩余几张 
public:
	train();
	
	train(const std::string &trainID_,const int &stationNum_,const sjtu::string *stations_,const int &seatNum,const int *prices_,const int *travelTimes_,
		  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_);
	
	~train();
	
	int find(const sjtu::string &s);
	
	void Update_ticket(const Date &date,const sjtu::string &l,const sjtu::string &r,const int &num);//更新车票，从date发车的火车有num张从l到r的车票被买了 
	
	int query_ticket(const Date &date,const sjtu::string &l,const sjtu::string &r);//询问从date发车的火车至多能买多少张从l到r的车票
	
	void check_ticket(const Date &date);//在某一用户退票后，需确认有哪些候票人能拿到票 
	
	Ticket get_ticket(const sjtu::string &l,const sjtu::string &r,const Date &date); 
	
	Date get_start(const sjtu::string &s,const bool fl=0); 
	
	sjtu::pair<int,Ticket> get_ticket2(const sjtu::string &l,const sjtu::string &r,const Date &date);
	
	friend class TrainManagement;
		
};

train::train(const std::string &trainID_,const int &stationNum_,const sjtu::string *stations_,const int &seatNum,const int *prices_,const int *travelTimes_,
	  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_):trainID(trainID_){
	stationNum=stationNum_;
	startTime=startTime_;
	saleDateL=saleDateL_;
	saleDateR=saleDateR_;
	Type=type_;
	for(int i=0;i<stationNum;i++){
		stations[i]=stations_[i];
		prices[i]=prices_[i];
		if(i<stationNum-1)travelTimes[i]=travelTimes_[i]; 
	}
	for(int i=0;i<stationNum-2;i++)stopoverTimes[i]=stopoverTimes_[i];
	for(int i=0,d=saleDateR-saleDateL;i<=d;i++)
		for(int j=0;j<stationNum;j++)LeftTicket[i][j]=seatNum;
	rel=0;
}

train::~train(){}

train::train():rel(0),stationNum(0),trainID(""){} 

Date train::get_start(const sjtu::string &s,const bool fl){
	Date ret;
	if(!fl)ret=Date(saleDateL.m,saleDateL.d,startTime.hr,startTime.mi);
	else ret=Date(saleDateR.m,saleDateR.d,startTime.hr,startTime.mi);
	for(int i=0;;i++){
		if(i>0)ret+=stopoverTimes[i-1];
		if(stations[i]==s)break;
		ret+=travelTimes[i];
	}
	return ret;
}

int train::find(const sjtu::string &s){
	for(int i=0;i<stationNum;i++)
		if(s==stations[i])return i;
	return -1;
}

void train::Update_ticket(const Date &date,const sjtu::string &l,const sjtu::string &r,const int &num){
	for(int now=date-get_start(l),i=find(l),R=find(r);i<R;i++)LeftTicket[now][i]-=num;
}

int train::query_ticket(const Date &date,const sjtu::string &l,const sjtu::string &r){
	int ret=2e9+233;
	for(int now=date-get_start(l),i=find(l),R=find(r);i<R;i++)
		if(LeftTicket[now][i]<ret)ret=LeftTicket[now][i];
	return ret;
}

Ticket train::get_ticket(const sjtu::string &l,const sjtu::string &r,const Date &date_){
	if(rel!=1)return Ticket("");
	int lx=find(l),rx=find(r);
	if(lx==-1||rx==-1||rx<lx)return Ticket("");
	Date date(date_);
	Date L=get_start(l),R=get_start(l,1);
	if(date<L||R<date)return Ticket(sjtu::string(""));
	Ticket ret;
	int k=date-L; 
	date.hr=L.hr,date.mi=L.mi;
	ret.trainID=trainID;
	ret.From=l;
	ret.To=r;
	ret.TimeL=date;
	for(int st=find(l),i=st;stations[i]!=r;i++){
		ret.cost+=prices[i];
		if(ret.num>LeftTicket[k][i])ret.num=LeftTicket[k][i];
		if(i>st){
			date+=stopoverTimes[i-1];
			ret.time+=stopoverTimes[i-1];
		}
		date+=travelTimes[i];
		ret.time+=travelTimes[i];
	}
	ret.TimeR=date; 
	return ret;
}

sjtu::pair<int,Ticket> train::get_ticket2(const sjtu::string &l,const sjtu::string &r,const Date &date_){
	if(rel!=1)return sjtu::pair<int,Ticket>(-1,Ticket(""));
	int lx=find(l),rx=find(r);
	if(lx==-1||rx==-1||rx<lx)return sjtu::pair<int,Ticket>(-1,Ticket(""));
	Date date(date_); 
	Date L=get_start(l),R=get_start(l,1);
	if(R<date)return sjtu::pair<int,Ticket>(-1,Ticket(""));
	int Ret=0;
	if(date<L){
		Ret=(L-date)*24*60-date.change(1)+L.change(1);
		date=L;
	}else{
		if( date.Compare(L) ){
			Ret=L.del(date);
			date.hr=L.hr,date.mi=L.mi;
		}else {
			Date last(L);
			L+=stopoverTimes[find(l)];
			if(date.Compare(L))L=last;
			else {
				L=last;
				date+=60*24;
				if(R<date)return sjtu::pair<int,Ticket>(-1,Ticket(""));
				Ret=date.change(0)+L.change(1);
				date.hr=L.hr,date.mi=L.mi;
			}
		}
	}
	Ticket ret;
	int k=date-L; 
	ret.trainID=trainID;
	ret.From=l;
	ret.To=r;
	ret.TimeL=date;
	for(int st=find(l),i=st;stations[i]!=r;i++){
		ret.cost+=prices[i];
		if(ret.num>LeftTicket[k][i])ret.num=LeftTicket[k][i];
		if(i>st){
			date+=stopoverTimes[i-1];
			ret.time+=stopoverTimes[i-1];
		}
		date+=travelTimes[i];
		ret.time+=travelTimes[i];
	}
	ret.TimeR=date; 
	return sjtu::pair<int,Ticket>(Ret,ret);
}

class TrainManagement{
private:
	sjtu::bpt<sjtu::string,train> pos;
	sjtu::bpt< type3 ,int> Road;
	sjtu::vector<sjtu::string> Train[M*M*M*3];
public:
	TrainManagement();
	
	~TrainManagement();
	
	void add_train(const std::string &trainID_,const int &stationNum_,const sjtu::string *stations_,const int &seatNum,const int *prices_,const int *travelTimes_,
	  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_);//add_train command 
	
	void release_train(const std::string &trainID);//release_train command 
	
	void delete_train(const std::string &trainID);//delete_train command
	
	void query_train(const std::string &trainID,const Date &date);//query_train command 
	
	void query_ticket(const std::string &From,const std::string &To,const Date &date,bool flag);//query_ticket command 
	
	void query_transfer(const std::string &From,const std::string &To,const Date &date,bool flag);//query_transfer command 
	
	Ticket get_ticket(const sjtu::string &trainID,const Date &date,const sjtu::string From,const sjtu::string To);
	
	void Update_ticket(const Ticket &t);
	
	Transfer_Ticket get_second(const Ticket &now,const sjtu::string To);
	
	void Reset();
};

TrainManagement::TrainManagement():pos("file_train","file_train_delete"),Road("file_Road","file_Road_delete"){
	if(Road.empty()){
		type3 res("","");
		sjtu::pair< type3,int >tmp(res,0);
		Road.insert(tmp);
	}
//	std::cerr<<"OK"<<std::endl; 
	if(!pos.empty()){
		sjtu::string last(""),now=pos.upper_bound(last);
		while(last!=now){
			train x=pos.find(now);
			last=now;
			now=pos.upper_bound(last);
			if(x.rel!=1)continue;
			std::cerr<<x.rel<<std::endl;
			for(int i=0;i<x.stationNum;i++)
				for(int j=i+1;j<x.stationNum;j++){
					type3 res(x.stations[i],x.stations[j]);
					if(!Road.count(res))std::cerr<<x.trainID<<std::endl; 
					else Train[ Road.find(res) ].push_back(last);
				}
		}
	}
}

TrainManagement::~TrainManagement(){}

void TrainManagement::add_train(const std::string &trainID_,const int &stationNum_,const sjtu::string *stations_,const int &seatNum,const int *prices_,const int *travelTimes_,
	  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_){
	train x(trainID_,stationNum_,stations_,seatNum,prices_,travelTimes_,stopoverTimes_,startTime_,saleDateL_,saleDateR_,type_);
	if(!pos.empty()&&pos.count(x.trainID)){
		if(pos.find(x.trainID).rel==2){
			pos.modify(x.trainID,x);
			return void(puts("0"));
		}else return void(puts("-1"));
	}
//	std::cerr<<trainID_<<std::endl;
	pos.insert(sjtu::pair<sjtu::string,train>(x.trainID,x));
	puts("0");
}

void TrainManagement::delete_train(const std::string &trainID_){
	sjtu::string trainID(trainID_);
	if(!pos.count(trainID) || pos.find(trainID).rel!=0 )return void(puts("-1"));
	std::cerr<<'#'<<trainID<<std::endl;
	train tmp=pos.find(trainID);
	tmp.rel=2;
	pos.modify(trainID,tmp);
	std::cerr<<'*'<<pos.find(trainID).rel<<std::endl;
	puts("0");
}

void TrainManagement::release_train(const std::string &trainID_){
	sjtu::string trainID(trainID_);
	if(!pos.count(trainID) || pos.find(trainID).rel!=0 )return void(puts("-1"));
	train x=pos.find(trainID);
	x.rel=1;
	pos.modify(trainID,x);
	puts("0");
	type3 st(sjtu::string(""),sjtu::string(""));
	int k=Road.find( st );
	for(int i=0;i<x.stationNum;i++)
		for(int j=i+1;j<x.stationNum;j++){
			type3 res(x.stations[i],x.stations[j]);
			if(!Road.count(res))Road.insert( sjtu::pair<type3,int>(res,++k) );
			Train[Road.find(res)].push_back(x.trainID);
		}
	Road.modify(st,k); 
}

void TrainManagement::query_train(const std::string &trainID_,const Date &date){
	sjtu::string trainID(trainID_);
	if(!pos.count(trainID)||pos.find(trainID).rel==2)return void(puts("-1"));
	train x=pos.find(trainID);
	if(x.saleDateR<date||date<x.saleDateL)return void(puts("-1"));
	std::cout<<trainID<<' '<<x.Type<<std::endl;
	int k=date-x.saleDateL,cost=0;
	Date now(date.m,date.d,x.startTime.hr,x.startTime.mi);
	for(int i=0;i<x.stationNum;i++){
		std::cout<<x.stations[i]<<' ';
		if(i==0){
			std::cout<<"xx-xx xx:xx -> ";
			now.print();
			now+=x.travelTimes[i];
		}else if(i==x.stationNum-1){
			cost+=x.prices[i-1];
			now.print();
			std::cout<<" -> xx-xx xx:xx";
		}else{
			now.print();
			std::cout<<" -> ";
			now+=x.stopoverTimes[i-1];
			now.print();
			now+=x.travelTimes[i];
			cost+=x.prices[i-1];
		}
		std::cout<<' '<<cost<<' ';
		if(i==x.stationNum-1)std::cout<<'x'<<std::endl;
		else std::cout<<x.LeftTicket[k][i]<<std::endl;
	}
}

Ticket TrainManagement::get_ticket(const sjtu::string &trainID,const Date &date,const sjtu::string From,const sjtu::string To){
	if(!pos.count(trainID))return Ticket("");
	return pos.find(trainID).get_ticket(From,To,date);
}

void TrainManagement::Update_ticket(const Ticket &t){
	train now=pos.find(t.trainID);
	now.Update_ticket(t.TimeL,t.From,t.To,t.num); 
	pos.modify(t.trainID,now);
//	std::cout<<"OK1"<<std::endl;
}

int ticket_num;
Ticket A[M*M];
bool (*cmp)(const Ticket &a,const Ticket &b);
bool (*Cmp)(const Transfer_Ticket &a,const Transfer_Ticket &b);
void sort(int l,int r){
	if(l>=r)return;
	Ticket key=A[l];
	int i=l,j=r;
	while(i<j){
		while(i<j&&!cmp(A[j],key))j--;
		while(i<j&&cmp(A[i],key))i++;
		if(i<j)std::swap(A[i],A[j]);
	}
	sort(l,i);
	sort(i+1,r);
}

void TrainManagement::query_ticket(const std::string &From,const std::string &To,const Date &date,bool flag){
	sjtu::pair<sjtu::string,sjtu::string> road(From,To);
	if(!Road.count(road))return void(puts("0"));
	int k=Road.find(road);
	ticket_num=0;
	for(int i=0;i<Train[k].size();i++){
		Ticket x=pos.find(Train[k][i]).get_ticket(From,To,date);
		if(x.trainID.size()==0)continue;
		A[++ticket_num]=x;
	}
	cmp=flag==0?cmp1:cmp2;
	sort(1,ticket_num);
	
	std::cout<<ticket_num<<std::endl;
	for(int i=1;i<=ticket_num;i++)A[i].print();
}

Transfer_Ticket TrainManagement::get_second(const Ticket &now,const sjtu::string To){
	sjtu::pair<sjtu::string,sjtu::string> road(now.To,To);
	if(!Road.count(road))return -1;
	int k=Road.find(road);
	Transfer_Ticket ret,tmp;
	bool First=1;
	for(int i=0;i<Train[k].size();i++){
		if(Train[k][i]==now.trainID)continue;
		 sjtu::pair<int,Ticket>B=pos.find(Train[k][i]).get_ticket2(now.To,To,now.TimeR);
		 if(B.first<0)continue;
		 tmp=Transfer_Ticket(now,B.second,B.first);
		 if(First)ret=tmp;
		 else if(Cmp(tmp,ret))ret=tmp;
		 First=0; 
	}
	if(First==1)return -1;
	return ret;
}

void TrainManagement::query_transfer(const std::string &From_,const std::string &To_,const Date &date,bool flag){
	sjtu::string last(""),now=pos.upper_bound(last),From(From_),To(To_);
	bool First=1;
	Cmp=flag==0?Cmp1:Cmp2;
	Transfer_Ticket ret,B;
	while(now!=last){
		train x=pos.find(now);
		last=now;
		now=pos.upper_bound(last);
		if(x.rel!=1||x.find(From)==-1)continue;
		if(date<x.get_start(From)||x.get_start(From,1)<date)continue;
		Date L=x.get_start(From),datenow(date);
		int k=datenow-L;
		datenow.hr=L.hr,datenow.mi=L.mi;
		Ticket fi;
		fi.trainID=last;
		fi.From=From;
		fi.TimeL=datenow;
		for(int i=x.find(From);i<x.stationNum-1;i++){
			fi.To=x.stations[i+1];
			fi.cost+=x.prices[i];
			if(fi.num>x.LeftTicket[k][i])fi.num=x.LeftTicket[k][i];
			datenow+=x.travelTimes[i];
			fi.time+=x.travelTimes[i];
			fi.TimeR=datenow;
			
			if(fi.To!=To){
				B=get_second(fi,To);
				if(B.time>=0){
					if(First)First=0,ret=B;
					else if(Cmp(B,ret))ret=B;
					First=0;
				}
			}
			
			if(i!=x.stationNum-1){
				fi.time+=x.stopoverTimes[i];
				datenow+=x.stopoverTimes[i];
			}
		}
	}
	if(First)puts("0");
	else{
		ret.A.print();
		ret.B.print(); 
	}
}

void TrainManagement::Reset(){
	pos.clean();
	Road.clean();
	for(int i=0;i<M*M*M>>1;i++)Train[i].clear();
}

