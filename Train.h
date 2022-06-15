#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

#include "string.hpp"
#include "vector.hpp"
#include "Ticket.h"
#include "linked_hashmap.hpp"


const int M = 105;
typedef sjtu::pair<size_t, size_t> type3;

class TrainManagement;

struct Basictrain{
    int maxday,maxseat;
    bool rel;
    Basictrain(){}

    Basictrain(const int &day,const int &seat,const int Rel=0):maxday(day),maxseat(seat),rel(Rel){}
};

struct train{
    sjtu::string trainID;
    int stationNum,seatNum;
	sjtu::string stations[M];
	int prices[M],travelTimes[M],stopoverTimes[M];
	Date saleDateL,saleDateR;
	char Type; 
	bool rel; 

	train(){}
	
	train(const sjtu::string &trainID_,const int &stationNum_,const sjtu::string *stations_,const int &seatNum_,const int *prices_,const int *travelTimes_,
		  const int *stopoverTimes_,const Date &startTime_,const Date &saleDateL_,const Date &saleDateR_,const char &type_):trainID(trainID_),stationNum(stationNum_),seatNum(seatNum_),rel(0){
        saleDateL=Date(saleDateL_.m,saleDateL_.d,startTime_.hr,startTime_.mi);
        saleDateR=Date(saleDateR_.m,saleDateR_.d,startTime_.hr,startTime_.mi);
        Type=type_;
        for(int i=0;i<stationNum;i++){
            stations[i]=stations_[i];
            
            if(i<stationNum-1){
                travelTimes[i]=travelTimes_[i]; 
                prices[i]=prices_[i];
            }
        }
        stopoverTimes[0]=stopoverTimes[stationNum-1]=0;
        for(int i=1;i<stationNum-1;i++)stopoverTimes[i]=stopoverTimes_[i-1];
    }
};

struct Stations {
    int pos,pri;
    Date ari,lef;
    Stations(){}
    
    Stations(const int &pos_,const int &pri_,const Date &ari_,const Date &lef_):pos(pos_),pri(pri_),ari(ari_),lef(lef_){}
};

class Left_Ticket {
private:
    int data[M][M];

public:
    Left_Ticket() {}

    Left_Ticket(const int &maxday, const int &stationNum, const int &seat) {
        for (int i = 0; i <= maxday; i++)
            for (int j = 0; j < stationNum; j++)
                data[i][j] = seat;
    }

    void Update_ticket(const int &k, const int &l, const int &r, const int &num) {
        for (int *x = data[k], i = l; i < r; i++)x[i] -= num;
    }

    int query_ticket(const int &k, const int &l, const int &r) const {
        int ret = 2e9;
        const int *x = data[k];
        for (int i = l; i < r; i++)
            if (x[i] < ret)ret = x[i];
        return ret;
    }

    friend class TrainManagement;
};

class TrainManagement {
private:   
    sjtu::linked_hashmap<size_t, Basictrain > bas;
    sjtu::linked_hashmap<size_t, sjtu::string> trainid;

    sjtu::bpt<size_t, train,254,4>tra;
    sjtu::bpt<size_t, Left_Ticket,254,4> Tk;
    sjtu::bpt<type3, Stations,169,72> sta;

    sjtu::bpt<type1, Ticket ,169,19> Get_ticket;//(username,kth) -> Ticket
    sjtu::bpt<sjtu::pair<type1,int>, Pendtype,127,85> inqu_ticket;//((trainID,kth),timestamp) -> (username,Ticket)
    sjtu::bpt<size_t, int,254,254> Num;//username -> tt

public:
    TrainManagement();

    ~TrainManagement();

    void
    add_train(const std::string &trainID_, const int &stationNum_, const sjtu::string *stations_, const int &seatNum,
              const int *prices_, const int *travelTimes_,
              const int *stopoverTimes_, const Date &startTime_, const Date &saleDateL_, const Date &saleDateR_,
              const char &type_);//add_train command

    void release_train(const std::string &trainID, const int &timestamp);//release_train command

    void delete_train(const std::string &trainID);//delete_train command

    void query_train(const std::string &trainID, const Date &date);//query_train command

	
	int query_ticket(const sjtu::string &trainID_, const Date &L, const sjtu::string From_,const sjtu::string To_);

    void query_transfer(const std::string &From, const std::string &To, const Date &date,
                        bool flag);//query_transfer command

    void Update_ticket(const Ticket &t);

    void Reset();

    void query_ticket(const std::string &From, const std::string &To, const Date &date, bool flag);

    sjtu::pair<int, Ticket2>get_ticket2(const sjtu::string trainID_, const Stations &Lx, const Stations &Rx,const Date &date_);

    Ticket2 get_ticket(const sjtu::string &trainID_,const Stations &L,const Stations &R,const Date &date);
    
    void buy_ticket(const std::string &username,const std::string &trainID_, const Date &date, const std::string From_,
                                 const std::string To_,const int &ask,const bool &flag,const int &timestamp);

    void Update_ticket(const size_t &trainID,const int &k,const int &l,const int &r,const int &num);

    int insert_ticket(const size_t &username_, const int &timestamp, const Ticket &t);

    void query_order(const size_t &username);
    
    void refund_ticket(const size_t &username, const int &k);

    void Print();
};

void TrainManagement::Print(){
    /*tra.print();
    Tk.print();
    sta.print();
    Get_ticket.print();
    inqu_ticket.print();
    Num.print();*/
}

int TrainManagement::query_ticket(const sjtu::string &trainID_, const Date &date, const sjtu::string From_,const sjtu::string To_) {
    size_t trainID = H(trainID_.change()),From=H(From_.change()),To=H(To_.change());
    auto L=sta.find(type3(From,trainID)),R=sta.find(type3(To,trainID));
    return Tk.find(trainID).query_ticket(date-L.lef,L.pos,R.pos);
}

TrainManagement::TrainManagement() : tra("file_train.dat","file_train2.dat"/*, "file_train_delete.dat","file_train2_delete.dat"*/),
                                     sta("file_sta.dat","file_sta2.dat"/*, "file_sta_delete.dat","file_sta2_delete.dat"*/),
                                     Tk("file_Ticket.dat","file_Ticket2.dat"/*, "file_Ticket_delete.dat","file_Ticket2_delete.dat"*/),

                                     Get_ticket("file_get_ticket.dat","file_get_ticket2.dat"/*, "file_get_ticket_delete.dat","file_get_ticket2_delete.dat"*/),
                                     inqu_ticket("file_inqu_ticket.dat","file_inqu_ticket2.dat"/*, "file_inqu_ticket_delete.dat","file_inqu_ticket2_delete.dat"*/),
                                     Num("file_num.dat","file_num2.dat"/*, "file_num_delete.dat","file_num2_delete.dat"*/){
                                        if(tra.empty())return;
                                        const sjtu::vector<sjtu::pair<size_t,train> > &vec=tra.traverse_val(0,(size_t)-1);
                                        for(int i=0;i<(int)vec.size();i++){
                                            const train &x=vec[i].second;
                                            bas[vec[i].first]=Basictrain(x.saleDateR-x.saleDateL,x.seatNum,x.rel);
                                            trainid[vec[i].first]=x.trainID;
                                        }
                                     }

TrainManagement::~TrainManagement() {}

void TrainManagement::add_train(const std::string &trainID_, const int &stationNum_, const sjtu::string *stations_,
                                const int &seatNum, const int *prices_, const int *travelTimes_,
                                const int *stopoverTimes_, const Date &startTime_, const Date &saleDateL_,
                                const Date &saleDateR_, const char &type_) {
    size_t trainID = H(trainID_);
    if(bas.count(trainID))return void(puts("-1"));
    bas[trainID]=Basictrain(saleDateR_-saleDateL_,seatNum);
    trainid[trainID]=sjtu::string(trainID_);
    tra.insert(trainID,train(sjtu::string(trainID_),stationNum_,stations_,seatNum,prices_,travelTimes_,stopoverTimes_,startTime_,saleDateL_,saleDateR_,type_));
    Tk.insert(trainID,Left_Ticket(saleDateR_-saleDateL_,stationNum_,seatNum));
    puts("0");
}

void TrainManagement::delete_train(const std::string &trainID_) {
    size_t trainID = H(trainID_);
    if(!bas.count(trainID)||bas[trainID].rel==1)return void(puts("-1"));
    tra.erase(trainID);
    bas.erase(bas.find(trainID));
    trainid.erase(trainid.find(trainID));
    Tk.erase(trainID);
    puts("0");
}

void TrainManagement::release_train(const std::string &trainID_, const int &timestamp) {
    size_t trainID = H(trainID_);
    if(!bas.count(trainID)||bas[trainID].rel==1)return void(puts("-1"));
    train x=tra.find(trainID);
    Date date(x.saleDateL);
    int res=0;
    for(int i=0;i<x.stationNum;i++){
        sta.insert(type3(H(x.stations[i].change()),trainID),Stations(i,res,date,date+x.stopoverTimes[i]));
        date+=x.stopoverTimes[i];
        if(i!=x.stationNum-1)date+=x.travelTimes[i],res+=x.prices[i];
    }
    puts("0");
    x.rel=1;
    tra.modify(trainID,x);
    bas[trainID].rel=1;
}

void TrainManagement::query_train(const std::string &trainID_,const Date &date){
	size_t trainID=H(trainID_);
    if(!bas.count(trainID))return void(puts("-1"));
	const train &x=tra.find(trainID);
	if(x.saleDateR<date||date<x.saleDateL)return void(puts("-1"));
    std::cout<<trainID_;
    printf(" %c\n",x.Type);
	int k=date-x.saleDateL,cost=0;
    const Left_Ticket &tk=Tk.find(trainID);
	Date now(date.m,date.d,x.saleDateL.hr,x.saleDateL.mi);
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
			now+=x.stopoverTimes[i];
			now.print();
			now+=x.travelTimes[i];
			cost+=x.prices[i-1];
		}
		printf(" %d ",cost); 
		if(i==x.stationNum-1)puts("x");
		else printf("%d\n",tk.data[k][i]);
	}
}

Ticket2 TrainManagement::get_ticket(const sjtu::string &trainID_,const Stations &L,const Stations &R,const Date &date){
    size_t trainID=H(trainID_.change());
    int k=date-L.lef;
    if(k<0||k>bas[trainID].maxday)return Ticket2();
    return Ticket2(trainID_,L.lef+k*1440,R.ari+k*1440,R.pri-L.pri,Tk.find(trainID).query_ticket(k,L.pos,R.pos),R.ari.get_time(L.lef));
}

void TrainManagement::Update_ticket(const size_t &trainID,const int &k,const int &l,const int &r,const int &num){
    auto now=Tk.find(trainID);
    now.Update_ticket(k,l,r,num);
    Tk.modify(trainID,now);
}

void TrainManagement::buy_ticket(const std::string &username,const std::string &trainID_, const Date &date, const std::string From_,
                                 const std::string To_,const int &ask,const bool &flag,const int &timestamp) {
    size_t trainID = H(trainID_),From=H(From_),To=H(To_);
    if(!bas.count(trainID)||bas[trainID].maxseat<ask)return void(puts("-1"));
    auto L=sta.find2(type3(From,trainID)),R=sta.find2(type3(To,trainID));
    if(L.first==0||R.first==0||L.second.pos>R.second.pos)return void(puts("-1"));
    Ticket2 tk=get_ticket(sjtu::string(trainID_),L.second,R.second,date);
    if(tk.cost==-1)return void(puts("-1"));//时间超
    if(flag==0&&tk.num<ask)return void(puts("-1"));//不候补
    int k=date-L.second.lef;
    if(tk.num>=ask){
        tk.num=ask;
        Update_ticket(trainID,k,L.second.pos,R.second.pos,ask);
        insert_ticket(H(username),timestamp,Ticket(tk,From_,To_,timestamp));
        printf("%lld\n",1LL*ask*tk.cost);
    }else{
        tk.num=ask;
        int id=insert_ticket(H(username),timestamp,Ticket(tk,From_,To_,-timestamp));
        inqu_ticket.insert(sjtu::pair<type1,int>(type1(trainID,k),timestamp),Pendtype(H(username),ask,L.second.pos,R.second.pos,id) );
        puts("queue");
    }

}

void TrainManagement::Update_ticket(const Ticket &t) {
    size_t trainID = H(t.trainID.change()),From=H(t.From.change()),To=H(t.To.change());
    auto now = Tk.find(trainID);
    auto L=sta.find(type3(From,trainID)),R=sta.find(type3(To,trainID));
    now.Update_ticket(t.TimeL - L.lef, L.pos, R.pos, t.num);
    Tk.modify(trainID, now);
}

int ticket_num;
Ticket2 A[M * M];

bool (*cmp)(const Ticket2 &a, const Ticket2 &b);

bool (*Cmp)(const Transfer_Ticket &a, const Transfer_Ticket &b);

void sort(int l, int r) {
    if (l >= r)return;
    Ticket2 key = A[l];
    int i = l, j = r;
    while (i < j) {
        while (i < j && !cmp(A[j], key))j--;
        while (i < j && cmp(A[i], key))i++;
        if (i < j)std::swap(A[i], A[j]);
    }
    sort(l, i);
    sort(i + 1, r);
}

void TrainManagement::query_ticket(const std::string &From, const std::string &To, const Date &date, bool flag) {
    if (sta.empty())return void(puts("0"));
    ticket_num = 0;
    const sjtu::vector<sjtu::pair<type3,Stations> > &vec1=sta.traverse_val(type3(H(From),0),type3(H(From),(size_t)-1));
    const sjtu::vector<sjtu::pair<type3,Stations> > &vec2=sta.traverse_val(type3(H(To),0),type3(H(To),(size_t)-1));
    if(vec1.size()==0||vec2.size()==0)return void(puts("0"));
    for(int i=0,j=0,sz=vec2.size();i<(int)vec1.size();i++){
        while(j+1<sz&&vec2[j].first.second<vec1[i].first.second)j++;
        if(vec2[j].first.second==vec1[i].first.second){
            const Stations &L=vec1[i].second,&R=vec2[j].second;
            if(L.pos<R.pos){
                size_t trainID=vec1[i].first.second;
                const Ticket2 &tik=get_ticket(trainid[trainID],L,R,date);
                if(tik.cost!=-1)A[++ticket_num]=tik;
            }
        }
    }

    cmp = flag == 0 ? cmp1 : cmp2;
    sort(1, ticket_num);

    printf("%d\n", ticket_num);
    for (int i = 1; i <= ticket_num; i++)A[i].print(From,To);
}

sjtu::pair<int, Ticket2>
TrainManagement::get_ticket2(const sjtu::string trainID_, const Stations &Lx, const Stations &Rx,const Date &date_) {
    size_t trainID = H(trainID_.change());
    if(Lx.pos>Rx.pos)return sjtu::pair<int, Ticket2>(-1, Ticket2());
    Date date(date_);
    Date L = Lx.lef, R = Lx.lef+bas[trainID].maxday*1440;

    if (R < date)return sjtu::pair<int, Ticket2>(-1, Ticket2());
    int Ret = 0;
    if (date < L) {
        Ret = (L - date) * 1440 - date.change(1) + L.change(1);
        date = L;
    } else {
        if (date.Compare(L)) {
            Ret = L.del(date);
            date.hr = L.hr, date.mi = L.mi;
        } else {
            date += 60 * 24;
            if (R < date)return sjtu::pair<int, Ticket2>(-1, Ticket2());
            Ret = date.change(0) + L.change(1);
            date.hr = L.hr, date.mi = L.mi;
        }
    }
    int mi=Rx.ari.get_time(Lx.lef);
    date.hr = L.hr, date.mi = L.mi;
    return sjtu::pair<int, Ticket2>(Ret, Ticket2(trainID_, date, date + mi, Rx.pri-Lx.pri,
                                               Tk.find(trainID).query_ticket(date - L, Lx.pos, Rx.pos), mi));
}

void TrainManagement::query_transfer(const std::string &From, const std::string &To, const Date &date, bool flag) {
    
    if (sta.empty())return void(puts("0"));
    ticket_num = 0;
    const sjtu::vector<sjtu::pair<type3,Stations> > &vec1=sta.traverse_val(type3(H(From),0),type3(H(From),(size_t)-1));
    const sjtu::vector<sjtu::pair<type3,Stations> > &vec2=sta.traverse_val(type3(H(To),0),type3(H(To),(size_t)-1));

    bool First = 1;
    Cmp = flag == 0 ? Cmp1 : Cmp2;
    Transfer_Ticket ret, TMP;

    for(int i=0;i<(int)vec1.size();i++){
        const Stations &L=vec1[i].second;
        const train &now = tra.find(vec1[i].first.second);
        const sjtu::string &trainID=now.trainID;
        for(int j=L.pos+1;j<now.stationNum;j++){
            const std::string Mid=now.stations[j].change();
            if(Mid==To)continue;
            const size_t &now_station=H( Mid );
            const Ticket2 &Tmp=get_ticket(trainID,L,sta.find(type3(now_station, vec1[i].first.second)),date);
            if(Tmp.cost==-1)continue;
            Ticket A(Tmp ,From, now.stations[j].change() );
            for(int k=0;k<(int)vec2.size();k++){
                auto R=sta.find2(type3(now_station,vec2[k].first.second));
                if(R.first==0||vec2[k].first.second==vec1[i].first.second)continue;
                sjtu::pair<int,Ticket2> B=get_ticket2(trainid[vec2[k].first.second], R.second , vec2[k].second ,A.TimeR);
                if(B.first!=-1){
                    TMP=Transfer_Ticket(A,Ticket(B.second,Mid,To),B.first);
                    if(First)ret=TMP,First=0;
                    else if(Cmp(TMP,ret))ret=TMP;
                }
            }

        }

    }
    if (First)puts("0");
    else {
        ret.A.print();
        ret.B.print();
    }
}

void TrainManagement::Reset() {
    tra.clean();
    sta.clean();
    Tk.clean();

    inqu_ticket.clean();
    Get_ticket.clean();
    Num.clean();
}

int TrainManagement::insert_ticket(const size_t &username, const int &timestamp, const Ticket &t){
    if (Num.empty())Num.insert(username,0);
    auto now=Num.find2(username);
    if(now.first==0){
        Num.insert(username,0);
        now=Num.find2(username);
    }
    int tt = now.second;
    Get_ticket.insert(sjtu::pair<type1, Ticket>(type1(username, ++tt), t));
    Num.modify(username, tt);
    return tt;
}

void TrainManagement::query_order(const size_t &username) {
    if (Get_ticket.empty())return void(puts("0"));
    const sjtu::vector<Ticket> &vec = Get_ticket.traverse(type1(username, 0), type1(username, 2e9));
    printf("%d\n", (int) vec.size());
    for (int i = (int) vec.size() - 1; i >= 0; i--) {
        if (vec[i].time >0)printf("[success] ");
        else if (vec[i].time <0)printf("[pending] ");
        else printf("[refunded] ");
        vec[i].print();
    }
}

void TrainManagement::refund_ticket(const size_t &username, const int &k) {
    if (Num.empty() )return void(puts("-1"));
    auto tmp = Num.find2(username);
    if(tmp.first==0||k>tmp.second)return void(puts("-1"));
    int tt=tmp.second;
    Ticket t = Get_ticket.find(type1(username, tt - k + 1));
    if (t.time == 0)return void(puts("-1"));
    size_t trainID=H(t.trainID.change());
    auto Sta=sta.find(type3(H(t.From.change()),trainID));
    if(t.time<0){
        inqu_ticket.erase( sjtu::pair<type1,int>(type1(trainID,t.TimeL-Sta.lef),-t.time) );
        t.time=0;
        Get_ticket.modify(type1(username, tt - k + 1), t);
    }else{
        t.time=0;
        Get_ticket.modify(type1(username, tt - k + 1), t);
        t.num=-t.num,Update_ticket(t);
        const int &K=t.TimeL-Sta.lef;
        const type1 res(trainID,K);
        if(!inqu_ticket.empty()){
            const sjtu::vector< sjtu::pair<sjtu::pair<type1,int>,Pendtype> > &vec=inqu_ticket.traverse_val(  sjtu::pair<type1,int>(res,0),sjtu::pair<type1,int>(res,2e9)  );
            Left_Ticket tk=Tk.find(trainID);
            for(int i=0;i<(int)vec.size();i++){
                const Pendtype &x=vec[i].second;
                if(tk.query_ticket(K,x.l,x.r)>=x.num){
                    tk.Update_ticket(K,x.l,x.r,x.num); 
                    auto us=Get_ticket.find(type1(x.username,x.kth));
                    us.time=-us.time;
                    Get_ticket.modify(type1(x.username,x.kth),us);
                    inqu_ticket.erase(vec[i].first);
                    //std::cerr<<"OK"<<' '<<k<<' '<<i<<' '<<inqu_ticket.count(vec[i].first)<<std::endl;
                }
            }
            Tk.modify(trainID,tk);
        }
    }
    puts("0");
}