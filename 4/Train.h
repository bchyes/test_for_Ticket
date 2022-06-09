#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "string.hpp"
#include "vector.hpp"
#include "Ticket.h"
#include "linked_hashmap.hpp"

const int M=105;
typedef sjtu::pair<sjtu::string,sjtu::string> type3;

class TrainManagement;

struct trainNode{
	sjtu::string trainID;
	sjtu::linked_hashmap<std::string,int>pos;
	int day,sum_cost[M],sum_tr[M],sum_st[M],Seat;
	Date st[M];
	inline int find(const sjtu::string &s){	
		std::string a=s.change();
		if( !pos.count(a) )return -1;
		return pos[a];
	}
}T[10005];
sjtu::linked_hashmap<std::string,int>Pos;
int tt;

class Left_Ticket{
private:
	int data[M][M];

public:
	Left_Ticket(){}
	Left_Ticket(const int &maxday,const int &stationNum,const int &seat){
		for(int i=0;i<=maxday;i++)
			for(int j=0;j<stationNum;j++)
				data[i][j]=seat;
	}
	void Update_ticket(const int &k,const int &l,const int &r,const int &num){
		for(int *x=data[k],i=l;i<r;i++)x[i]-=num;
	}

	int query_ticket(const int &k,const int &l,const int &r){
		int ret=2e9;
		for(int i=l,*x=data[k];i<r;i++)
			if(x[i]<ret)ret=x[i];
		return ret;
	}

	friend class TrainManagement;
};

class train{
private:
	sjtu::string trainID;
	int stationNum,seatNum;
	sjtu::string stations[M];
	int prices[M],travelTimes[M],stopoverTimes[M];
	Date startTime,saleDateL,saleDateR;
	char Type;
	
public:
	train();
	
	train(const std::string &trainID_,const int &stationNum_,const int &seatNum_,const sjtu::string *stations_,const int *prices_,const int *travelTimes_,
		  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_);
	
	~train();
	
	friend class TrainManagement;
};

train::train(const std::string &trainID_,const int &stationNum_,const int &seatNum_,const sjtu::string *stations_,const int *prices_,const int *travelTimes_,
	  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_):trainID(trainID_),stationNum(stationNum_),seatNum(seatNum_){
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
}

train::~train(){}

train::train():trainID(""),stationNum(0){} 

class TrainManagement{
private:
	sjtu::bpt<sjtu::string,train> pos;
	sjtu::bpt< type3 ,int> Road;
	sjtu::bpt<sjtu::pair<int,int> ,sjtu::string> Train;
	sjtu::bpt<sjtu::string,Left_Ticket> Tk;
	sjtu::bpt<sjtu::string,int> rel;

public:
	TrainManagement();
	
	~TrainManagement();
	
	void add_train(const std::string &trainID_,const int &stationNum_,const sjtu::string *stations_,const int &seatNum,const int *prices_,const int *travelTimes_,
	  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_);//add_train command 
	
	void release_train(const std::string &trainID,const int &timestamp);//release_train command 
	
	void delete_train(const std::string &trainID);//delete_train command
	
	void query_train(const std::string &trainID,const Date &date);//query_train command 
	
	void query_ticket(const std::string &From,const std::string &To,const Date &date,bool flag);//query_ticket command 
	
	void query_transfer(const std::string &From,const std::string &To,const Date &date,bool flag);//query_transfer command 
	
	Ticket get_ticket(const sjtu::string &trainID,const Date &date,const sjtu::string From,const sjtu::string To,const int &ask=0);
	
	void Update_ticket(const Ticket &t);
	
	Transfer_Ticket get_second(const Ticket &now,const sjtu::string To);
	
	void Reset();
	
	void insert(const sjtu::string &trainID); 
	
	int query_ticket(const sjtu::string &trainID,const Date &L,const sjtu::string From,const sjtu::string To);
	
	sjtu::pair<int,Ticket> get_ticket2(const sjtu::string trainID,const sjtu::string &l_,const sjtu::string &r_,const Date &date_); 
};

int TrainManagement::query_ticket(const sjtu::string &trainID,const Date &L,const sjtu::string From,const sjtu::string To){
	int id=Pos[trainID.change()],l=T[id].find(From),r=T[id].find(To);
	return Tk.find(trainID).query_ticket(L-T[id].st[l],l,r);
}

void TrainManagement::insert(const sjtu::string &trainID_){
	train x=pos.find(trainID_);
	Pos[x.trainID.change()]=++tt;
	T[tt].trainID=trainID_; 
	T[tt].Seat=x.seatNum;
	for(int i=0;i<x.stationNum;i++)
		T[tt].pos[x.stations[i].change()]=i;
	 T[tt].day=(x.saleDateR-x.saleDateL)*60*24;
	 int *sum=T[tt].sum_cost;
	 sum[0]=0; 
	 for(int i=1;i<=x.stationNum;i++)
	 	sum[i]=sum[i-1]+x.prices[i-1];
	
	sum=T[tt].sum_tr;
	sum[0]=0;
	 for(int i=1;i<=x.stationNum;i++)
	 	sum[i]=sum[i-1]+x.travelTimes[i-1];
	
	sum=T[tt].sum_st;
	sum[0]=0;
	 for(int i=1;i<x.stationNum;i++)
	 	sum[i]=sum[i-1]+x.stopoverTimes[i-1];
	
	Date date(x.saleDateL.m,x.saleDateL.d,x.startTime.hr,x.startTime.mi);
	for(int i=0;;i++){
		if(i>0&&i!=x.stationNum-1)date+=x.stopoverTimes[i-1];
		T[tt].st[i]=date;
		if(i==x.stationNum-1)break;
		date+=x.travelTimes[i];
	}
}

TrainManagement::TrainManagement(): pos("file_train.dat","file_train_delet.dat"),
									Road("file_Road.dat","file_Road_delete.dat"),
									Train("file_Train_.dat","file_Train__delete.dat"),
									Tk("file_Ticket.dat","file_Ticket_delete.dat"),
									rel("file_rel.dat","file_rel_delete.dat"){
	if(Road.empty()){
		type3 res("","");
		sjtu::pair< type3,int >tmp(res,0);
		Road.insert(tmp);
	}
	tt=0;
	if(!pos.empty()){
		sjtu::string last(""),now=pos.upper_bound(last);
		while(now!=last){
			last=now,now=pos.upper_bound(last);
			if(rel.find(last)!=1)continue;
			insert(last);
		}
	}
}

TrainManagement::~TrainManagement(){}

void TrainManagement::add_train(const std::string &trainID_,const int &stationNum_,const sjtu::string *stations_,const int &seatNum,const int *prices_,const int *travelTimes_,
	  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_){
	train x(trainID_,stationNum_,seatNum,stations_,prices_,travelTimes_,stopoverTimes_,startTime_,saleDateL_,saleDateR_,type_);
	if(!pos.empty()&&pos.count(x.trainID)){
		if(rel.find(x.trainID)==2){
			pos.modify(x.trainID,x);
			Tk.modify(x.trainID,Left_Ticket(saleDateR_-saleDateL_,stationNum_,seatNum));
			rel.modify(x.trainID,0);
			return void(puts("0"));
		}else return void(puts("-1"));
	}
	pos.insert(sjtu::pair<sjtu::string,train>(x.trainID,x));
	Tk.insert(sjtu::pair<sjtu::string,Left_Ticket>(x.trainID,Left_Ticket(saleDateR_-saleDateL_,stationNum_,seatNum)));
	rel.insert(sjtu::pair<sjtu::string,bool>(x.trainID,0));
	puts("0");
}

void TrainManagement::delete_train(const std::string &trainID_){
	sjtu::string trainID(trainID_);
	if(!rel.count(trainID) || rel.find(trainID)!=0 )return void(puts("-1"));
	rel.modify(trainID,2);
	puts("0");
}

void TrainManagement::release_train(const std::string &trainID_,const int &timestamp){
	sjtu::string trainID(trainID_);
	if(!rel.count(trainID) || rel.find(trainID)!=0 )return void(puts("-1"));
	rel.modify(trainID,1);
	insert(trainID_);
	type3 st(sjtu::string(""),sjtu::string(""));
	int k=Road.find( st );
	train x=pos.find(trainID);
	for(int i=0;i<x.stationNum;i++)
		for(int j=i+1;j<x.stationNum;j++){
			type3 res(x.stations[i],x.stations[j]);
			if(!Road.count(res))Road.insert( sjtu::pair<type3,int>(res,++k) );
			sjtu::pair<int,int> tmp(Road.find(res),timestamp);
			Train.insert(sjtu::pair< sjtu::pair<int,int>,sjtu::string >(tmp,trainID));
		}
	Road.modify(st,k); 
	puts("0");
}

void TrainManagement::query_train(const std::string &trainID_,const Date &date){
	sjtu::string trainID(trainID_);
	if(!rel.count(trainID)||rel.find(trainID)==2)return void(puts("-1"));
	const train &x=pos.find(trainID);
	if(x.saleDateR<date||date<x.saleDateL)return void(puts("-1"));
	trainID.print();
	printf(" %c\n",x.Type);
	int k=date-x.saleDateL,cost=0;
	Date now(date.m,date.d,x.startTime.hr,x.startTime.mi);
	const Left_Ticket &T=Tk.find(trainID);
	for(int i=0;i<x.stationNum;i++){
		x.stations[i].print();
		putchar(' ');
		if(i==0){
			printf("xx-xx xx:xx -> ");
			now.print();
			now+=x.travelTimes[i];
		}else if(i==x.stationNum-1){
			cost+=x.prices[i-1];
			now.print();
			printf(" -> xx-xx xx:xx");
		}else{
			now.print();
			printf(" -> ");
			now+=x.stopoverTimes[i-1];
			now.print();
			now+=x.travelTimes[i];
			cost+=x.prices[i-1];
		}
		printf(" %d ",cost); 
		if(i==x.stationNum-1)puts("x");
		else printf("%d\n",T.data[k][i]);
	}
}

Ticket TrainManagement::get_ticket(const sjtu::string &trainID,const Date &date_,const sjtu::string From,const sjtu::string To,const int &ask){
	if(!Pos.count(trainID.change()))return Ticket("");
	int id=Pos[trainID.change()];
	if(T[id].Seat<ask)return Ticket("");
	int l=T[id].find(From),r=T[id].find(To);
	if(l==-1||r==-1||r<l)return Ticket("");
	Date date(date_);
	Date L=T[id].st[l],R=T[id].st[l]+T[id].day;
	if(date<L||R<date)return Ticket("");
	Ticket ret;
	date.hr=L.hr,date.mi=L.mi;
	ret.trainID=trainID;
	ret.From=From;
	ret.To=To;
	ret.TimeL=date;
	ret.num=Tk.find(trainID).query_ticket(date-L,l,r);
	
	date+=T[id].sum_st[r-1]-T[id].sum_st[l];
	ret.time+=T[id].sum_st[r-1]-T[id].sum_st[l];
	date+=T[id].sum_tr[r]-T[id].sum_tr[l];
	ret.time+=T[id].sum_tr[r]-T[id].sum_tr[l];
	
	ret.TimeR=date; 
	ret.cost=T[id].sum_cost[r]-T[id].sum_cost[l];
	return ret;
}

void TrainManagement::Update_ticket(const Ticket &t){
	Left_Ticket now=Tk.find(t.trainID);
	int id=Pos[t.trainID.change()],l=T[id].find(t.From),r=T[id].find(t.To);
	now.Update_ticket(t.TimeL-T[id].st[l],l,r,t.num); 
	Tk.modify(t.trainID,now);
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
	sjtu::pair<int,int> Lx(k,0),Rx(k,2e9);
	const sjtu::vector<sjtu::string> &vec=Train.traverse(Lx,Rx);
	for(int i=0;i<(int)vec.size();i++){
		Ticket x=get_ticket(vec[i],date,From,To);
		if(x.trainID.size()==0)continue;
		A[++ticket_num]=x;
	} 
	cmp=flag==0?cmp1:cmp2;
	sort(1,ticket_num);
	
	printf("%d\n",ticket_num);
	for(int i=1;i<=ticket_num;i++)A[i].print();
}

sjtu::pair<int,Ticket> TrainManagement::get_ticket2(const sjtu::string trainID,const sjtu::string &l_,const sjtu::string &r_,const Date &date_){
	if(!Pos.count(trainID.change()))return sjtu::pair<int,Ticket>(-1,Ticket(""));
	int id=Pos[trainID.change()],l=T[id].find(l_),r=T[id].find(r_);
	if(l==-1||r==-1||r<l)return sjtu::pair<int,Ticket>(-1,Ticket(""));
	Date date(date_); 
	Date L=T[id].st[l],R=T[id].st[l]+T[id].day;
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
			date+=60*24;
			if(R<date)return sjtu::pair<int,Ticket>(-1,Ticket(""));
			Ret=date.change(0)+L.change(1);
			date.hr=L.hr,date.mi=L.mi;
		}
	}
	Ticket ret;
	date.hr=L.hr,date.mi=L.mi;
	ret.trainID=trainID;
	ret.From=l_;
	ret.To=r_;
	ret.TimeL=date;
	ret.num=Tk.find(trainID).query_ticket(date-L,l,r); 
	date+=T[id].sum_st[r-1]-T[id].sum_st[l];
	ret.time+=T[id].sum_st[r-1]-T[id].sum_st[l];
	
	date+=T[id].sum_tr[r]-T[id].sum_tr[l];
	ret.time+=T[id].sum_tr[r]-T[id].sum_tr[l];
	
	ret.TimeR=date; 
	ret.cost=T[id].sum_cost[r]-T[id].sum_cost[l];
	return sjtu::pair<int,Ticket>(Ret,ret);
}

Transfer_Ticket TrainManagement::get_second(const Ticket &now,const sjtu::string To){
	sjtu::pair<sjtu::string,sjtu::string> road(now.To,To);
	if(!Road.count(road))return -1;
	int k=Road.find(road);
	Transfer_Ticket ret,tmp;
	bool First=1;
	sjtu::pair<int,int> Lx(k,0),Rx(k,2e9);
	const sjtu::vector<sjtu::string> &vec=Train.traverse(Lx,Rx);
	for(int i=0;i<(int)vec.size();i++){
		sjtu::string id=vec[i];
		if(id==now.trainID)continue;
		sjtu::pair<int,Ticket>B=get_ticket2(id,now.To,To,now.TimeR);
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
	sjtu::string From(From_),To(To_);
	bool First=1;
	Cmp=flag==0?Cmp1:Cmp2;
	Transfer_Ticket ret,B;
	for(int id=1;id<=tt;id++){
		if(!T[id].pos.count(From_))continue;
		int l=T[id].find(From);
		if(date<T[id].st[l]||T[id].st[l]+T[id].day<date)continue;
		train x=pos.find(T[id].trainID);
		Date L=T[id].st[l],datenow(date);
		int k=datenow-L;
		datenow.hr=L.hr,datenow.mi=L.mi;
		Ticket fi;
		fi.trainID=T[id].trainID;
		fi.From=From;
		fi.TimeL=datenow;
		Left_Ticket tkt=Tk.find(T[id].trainID);
		for(int i=l;i<x.stationNum-1;i++){
			fi.To=x.stations[i+1];
			fi.cost+=x.prices[i];
			if(fi.num>tkt.data[k][i])fi.num=tkt.data[k][i];
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
			
			if(i+1!=x.stationNum-1){
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
	Train.clean();
}

