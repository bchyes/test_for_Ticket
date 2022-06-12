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
typedef sjtu::pair<size_t,size_t> type3;

class TrainManagement;

class Prices{
private:
	int prices[M];
public:
	Prices(){}

	Prices(const int *data,const int &sz){
		prices[0]=0;
		for(int i=1;i<sz;i++)prices[i]=prices[i-1]+data[i-1];
	}

	inline int Ask(const int &l,const int &r)const {
		return prices[r]-prices[l];
	}

	friend class TrainManagement;
}; 

class Times{
private:
	int travelTimes[M],stopoverTimes[M];
	Date st_L,st_R;

public:
	Times(){}

	Times(const int *Travel,const int *Stop,const Date &L,const Date &R,const int &sz):st_L(L),st_R(R){
		travelTimes[0]=stopoverTimes[0]=0;
		for(int i=1;i<sz;i++){
			travelTimes[i]=Travel[i-1]+travelTimes[i-1];
			stopoverTimes[i]=(i==sz-1?0:Stop[i-1])+stopoverTimes[i-1];
		}
	}

	inline Date Ask(const int &k,const bool fl=0)const {
		return (fl==0?st_L:st_R)+travelTimes[k]+stopoverTimes[k];
	}

	inline int CalcTimes(const int &l,const int &r)const {
		return travelTimes[r]-travelTimes[l]+stopoverTimes[r-1]-stopoverTimes[l];
	}

	friend class TrainManagement;
};

class Stations{
private:
	sjtu::string stations[M];
	int sz,Seat;
	char type;
public:
	Stations(){}

	Stations(const sjtu::string *data,const int &k,const char &Ty,const int &x):sz(k),Seat(x),type(Ty){
		for(int i=0;i<sz;i++)stations[i]=data[i];
	}

	inline int find(const sjtu::string &s)const {	
		for(int i=0;i<sz;i++)if(s==stations[i])return i;
		return -1;
	}
	friend class TrainManagement;
};

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

	int query_ticket(const int &k,const int &l,const int &r)const {
		int ret=2e9;
		const int *x=data[k];
		for(int i=l;i<r;i++)
			if(x[i]<ret)ret=x[i];
		return ret;
	}

	friend class TrainManagement;
};

class TrainManagement{
private:
	sjtu::bpt< size_t,sjtu::string >back;
	sjtu::bpt< sjtu::pair<type3,int> ,sjtu::string> Train;

	sjtu::bpt<size_t,Left_Ticket> Tk;
	sjtu::bpt<size_t,bool> rel;
	sjtu::bpt<size_t,Stations> sta;
	sjtu::bpt<size_t,Times> Ti;
	sjtu::bpt<size_t,Prices> pri;

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
	
	Ticket get_ticket(const sjtu::string &trainID,const Date &date,const sjtu::string From,const sjtu::string To);
	
	void Update_ticket(const Ticket &t);
	
	Transfer_Ticket get_second(const Ticket &now,const sjtu::string &To);
	
	void Reset();
	
	int query_ticket(const sjtu::string &trainID,const Date &L,const sjtu::string From,const sjtu::string To);
	
	sjtu::pair<int,Ticket> get_ticket2(const sjtu::string trainID,const sjtu::string &l_,const sjtu::string &r_,const Date &date_); 

	sjtu::pair<int,Ticket> Calc_Price(const sjtu::string &trainID,const Date &date_,const sjtu::string From,const sjtu::string To,const int &ask,const bool &fl);

	void Update_ticket2(const size_t &trainID,const int &k,const int &l,const int &r,const int &num);
};

int TrainManagement::query_ticket(const sjtu::string &trainID_,const Date &L,const sjtu::string From,const sjtu::string To){
	size_t trainID=H(trainID_.change());
	const Stations &stations=sta.find(trainID);
	int l=stations.find(From),r=stations.find(To);
	return Tk.find(trainID).query_ticket(L-Ti.find(trainID).Ask(l),l,r);
}

TrainManagement::TrainManagement(): back("file_back.dat","file_back_delete.dat"),
									Train("file_Train_.dat","file_Train__delete.dat"),

									Tk("file_Ticket.dat","file_Ticket_delete.dat"),
									rel("file_rel.dat","file_rel_delete.dat"),
									sta("file_sta.dat","file_sta_delete.dat"),
									Ti("file_Ti.dat","file_Ti_delete.dat"),
									pri("file_pri.dat","file_pri_delete.dat"){}

TrainManagement::~TrainManagement(){}

void TrainManagement::add_train(const std::string &trainID_,const int &stationNum_,const sjtu::string *stations_,const int &seatNum,const int *prices_,const int *travelTimes_,
	  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_){
	size_t trainID=H(trainID_);
	if(!rel.empty()&&rel.count(trainID))return void(puts("-1"));
	back.insert(sjtu::pair<size_t,sjtu::string>(trainID,sjtu::string(trainID_)));
	Tk.insert(sjtu::pair<size_t,Left_Ticket>(trainID,Left_Ticket(saleDateR_-saleDateL_,stationNum_,seatNum)));
	rel.insert(sjtu::pair<size_t,bool>(trainID,0));
	sta.insert(sjtu::pair<size_t,Stations>(trainID,Stations(stations_,stationNum_,type_,seatNum)));
	Date L(saleDateL_.m,saleDateL_.d,startTime_.hr,startTime_.mi),R(saleDateR_.m,saleDateR_.d,startTime_.hr,startTime_.mi);
	Ti.insert(sjtu::pair<size_t,Times>(trainID,Times(travelTimes_,stopoverTimes_,L,R,stationNum_)));
	pri.insert(sjtu::pair<size_t,Prices>(trainID,Prices(prices_,stationNum_)));
	puts("0");
}

void TrainManagement::delete_train(const std::string &trainID_){
	size_t trainID=H(trainID_);
	if(rel.empty()|| !rel.count(trainID) || rel.find(trainID)!=0 )return void(puts("-1"));
	rel.erase(trainID);
	sta.erase(trainID);
	Tk.erase(trainID);
	Ti.erase(trainID);
	pri.erase(trainID);
	back.erase(trainID);
	puts("0");
}

void TrainManagement::release_train(const std::string &trainID_,const int &timestamp){
	size_t trainID=H(trainID_);
	if(rel.empty() || !rel.count(trainID) || rel.find(trainID)!=0 )return void(puts("-1"));
	sjtu::string push(trainID_);
	rel.modify(trainID,1);
	type3 st(H(""),H(""));
	Stations x=sta.find(trainID);
	sjtu::pair<type3,int> res;
	res.second=timestamp;
	for(int i=0;i<x.sz;i++){
		res.first.first=H(x.stations[i].change());
		for(int j=i+1;j<x.sz;j++){
			res.first.second=H(x.stations[j].change());
			Train.insert(sjtu::pair< sjtu::pair<type3,int>,sjtu::string >(res,push));
		}
	}
	puts("0");
}

void TrainManagement::query_train(const std::string &trainID_,const Date &date){
	size_t trainID=H(trainID_);
	if(rel.empty() || !rel.count(trainID)||rel.find(trainID)==2)return void(puts("-1"));
	const Times &times=Ti.find(trainID);
	if(times.st_R<date||date<times.st_L)return void(puts("-1"));
	std::cout<<trainID_;
	const Stations stations=sta.find(trainID);
	printf(" %c\n",stations.type);
	Date date_L=times.st_L;
	int k=date-date_L;
	Date now(date.m,date.d,date_L.hr,date_L.mi),tmp;
	const Left_Ticket &T=Tk.find(trainID);
	const Prices &prices=pri.find(trainID);
	for(int i=0;i<stations.sz;i++){
		stations.stations[i].print();
		putchar(' ');
		if(i==0){
			printf("xx-xx xx:xx -> ");
			now.print();
		}else if(i==stations.sz-1){
			tmp=now+times.travelTimes[i]+times.stopoverTimes[i-1];
			tmp.print();
			printf(" -> xx-xx xx:xx");
		}else{
			tmp=now+times.travelTimes[i]+times.stopoverTimes[i-1];
			tmp.print();
			printf(" -> ");
			tmp=now+times.travelTimes[i]+times.stopoverTimes[i];
			tmp.print();
		}
		printf(" %d ",prices.prices[i]); 
		if(i==stations.sz-1)puts("x");
		else printf("%d\n",T.data[k][i]);
	}
}

Ticket TrainManagement::get_ticket(const sjtu::string &trainID_,const Date &date_,const sjtu::string From,const sjtu::string To){
	size_t trainID=H(trainID_.change());
	int l=sta.find(trainID).find(From),r=sta.find(trainID).find(To);
	const Times &times = Ti.find(trainID);
	const Date &L=times.Ask(l),&R=times.Ask(l,1);
	Date date(date_.m,date_.d,L.hr,L.mi);
	if(date<L||R<date)return Ticket("");
	int mi=times.CalcTimes(l,r);
	return Ticket(sjtu::string(trainID_),From,To,date,date+mi,pri.find(trainID).Ask(l,r),Tk.find(trainID).query_ticket(date-L,l,r),mi);
}

sjtu::pair<int,Ticket> TrainManagement::Calc_Price(const sjtu::string &trainID_,const Date &date_,const sjtu::string From,const sjtu::string To,const int &ask,const bool &fl){
	size_t trainID=H(trainID_.change());
	if(rel.empty()||!rel.count(trainID)||rel.find(trainID)!=1)return sjtu::pair<int,Ticket>(-1,"");
	const Stations &stations=sta.find(trainID);
	int l=stations.find(From),r=stations.find(To),mx=stations.Seat;
	if(mx<ask||l==-1||r==-1||l>r)return sjtu::pair<int,Ticket>(-1,"");
	const Times &times = Ti.find(trainID);
	const Date &L=times.Ask(l),R=times.Ask(l,1);
	if(date_<L||R<date_)return sjtu::pair<int,Ticket>(-1,"");
	const Date date(date_.m,date_.d,L.hr,L.mi);
	int tk=Tk.find(trainID).query_ticket(date-L,l,r);
	if(tk>=ask){
		Update_ticket2(trainID,date-L,l,r,ask);
		int mi=times.CalcTimes(l,r),cost=pri.find(trainID).Ask(l,r);
		printf("%lld\n",1LL*ask*cost);
		return sjtu::pair<int,Ticket>(0,Ticket(trainID_,From,To,date,date+mi,cost,ask));
	}else if(fl==0)return sjtu::pair<int,Ticket>(-1,"");
	else {
		puts("queue");
		int mi=times.CalcTimes(l,r),cost=pri.find(trainID).Ask(l,r);
		return sjtu::pair<int,Ticket>(1,Ticket(trainID_,From,To,date,date+mi,cost,ask));
	}
}

void TrainManagement::Update_ticket2(const size_t &trainID,const int &k,const int &l,const int &r,const int &num){
	Left_Ticket now=Tk.find(trainID);
	now.Update_ticket(k,l,r,num);
	Tk.modify(trainID,now);
}

void TrainManagement::Update_ticket(const Ticket &t){
	size_t trainID=H(t.trainID.change());
	Left_Ticket now=Tk.find(trainID);
	int l=sta.find(trainID).find(t.From),r=sta.find(trainID).find(t.To);
	now.Update_ticket(t.TimeL-Ti.find(trainID).Ask(l),l,r,t.num); 
	Tk.modify(trainID,now);
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
	if(Train.empty())return void(puts("0"));
	type3 res(H(From),H(To));
	ticket_num=0;
	const sjtu::vector<sjtu::string> &vec=Train.traverse(sjtu::pair<type3,int>(res,0),sjtu::pair<type3,int>(res,2e9));
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

sjtu::pair<int,Ticket> TrainManagement::get_ticket2(const sjtu::string trainID_,const sjtu::string &l_,const sjtu::string &r_,const Date &date_){
	size_t trainID=H(trainID_.change());
	int l=sta.find(trainID).find(l_),r=sta.find(trainID).find(r_);
	if(l==-1||r==-1||r<l)return sjtu::pair<int,Ticket>(-1,Ticket(""));

	const Times &times = Ti.find(trainID);
	Date date(date_);
	Date L=times.Ask(l),R=times.Ask(l,1);

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

	int mi=times.CalcTimes(l,r);
	date.hr=L.hr,date.mi=L.mi;
	return sjtu::pair<int,Ticket>(Ret,Ticket(trainID_,l_,r_,date,date+mi,pri.find(trainID).Ask(l,r),Tk.find(trainID).query_ticket(date-L,l,r),mi));
}

Transfer_Ticket TrainManagement::get_second(const Ticket &now,const sjtu::string &To){
	if(Train.empty())return -1;
	type3 res(H(now.To.change()),H(To.change()));
	Transfer_Ticket ret,tmp;
	bool First=1;
	const sjtu::vector<sjtu::string> &vec=Train.traverse(sjtu::pair<type3,int>(res,0),sjtu::pair<type3,int>(res,2e9));
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
	if(rel.empty()) return void(puts("0"));
	sjtu::string From(From_),To(To_);
	bool First=1;
	Cmp=flag==0?Cmp1:Cmp2;
	Transfer_Ticket ret,B;

	const sjtu::vector<sjtu::pair<size_t,bool> > &vec=rel.traverse_val(0,(size_t)(-1));
	for(int j=0;j<(int)vec.size();j++){
		if(vec[j].second==0)continue;
		size_t last=vec[j].first;
		
		const Stations &stations=sta.find(last);
		int l=stations.find(From);
		if(l==-1)continue;
		const Times &times=Ti.find(last);
		if(date<times.Ask(l)||times.Ask(l,1)<date)continue;
		const Left_Ticket &tkt=Tk.find(last);
		const Prices &prices=pri.find(last);
		Date L=times.Ask(l),datenow(date);
		int k=datenow-L;
		datenow.hr=L.hr,datenow.mi=L.mi;
		Ticket fi;
		fi.trainID=back.find(last);
		fi.From=From;
		fi.TimeL=datenow;
		for(int i=l;i<stations.sz-1;i++){
			fi.To=stations.stations[i+1];
			fi.cost=prices.Ask(l,i+1);
			if(fi.num>tkt.data[k][i])fi.num=tkt.data[k][i];
			fi.time=times.CalcTimes(l,i+1);
			fi.TimeR=fi.TimeL+fi.time;
			if(fi.To!=To){
				B=get_second(fi,To);
				if(B.time>=0){
					if(First)First=0,ret=B;
					else if(Cmp(B,ret))ret=B;
					First=0;
				}
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
	back.clean();
	Train.clean();
	
	rel.clean();
	Tk.clean();
	Ti.clean();
	pri.clean();
	sta.clean();
}

