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

struct Ticket2{
    sjtu::string trainID;
    Date TimeL,TimeR;
    int cost,time,num;
    Ticket2():cost(-1){};

    Ticket2(const sjtu::string &id, const Date &l, const Date &r,
           const int &cost_, const int &Num, const int tm = 0) :
            trainID(id),
            TimeL(l), TimeR(r),
            cost(cost_), time(tm), num(Num) {}
    void print(const std::string &f,const std::string &t)const {
        trainID.print();
        putchar(' ');
        std::cout<<f;
        putchar(' ');
        TimeL.print();
        printf(" -> ");
        std::cout<<t;
        putchar(' ');
        TimeR.print();
        printf(" %d %d\n", cost, num);

    }
};

struct Ticket {
    sjtu::string trainID, From, To;
    Date TimeL, TimeR;
    int cost, time, num;

    Ticket(const Ticket2 &a,const std::string &f,const std::string &t):
        trainID(a.trainID),From(f),To(t),TimeL(a.TimeL),TimeR(a.TimeR),cost(a.cost),time(a.time),num(a.num){}

    
    Ticket(const Ticket2 &a,const std::string &f,const std::string &t,const int &fl):
        trainID(a.trainID),From(f),To(t),TimeL(a.TimeL),TimeR(a.TimeR),cost(a.cost),time(fl),num(a.num){}

    Ticket() : cost(0), time(0), num(2e9) {}

    Ticket(const sjtu::string &res) : trainID(res) {}

    Ticket(const std::string &res) : trainID(res) {}

    Ticket(const Ticket &res) :
            trainID(res.trainID), From(res.From), To(res.To),
            TimeL(res.TimeL), TimeR(res.TimeR),
            cost(res.cost), time(res.time), num(res.num) {}

    Ticket(const sjtu::string &id, const sjtu::string &f, const sjtu::string &t, const Date &l, const Date &r,
           const int &cost_, const int &Num, const int tm = 0) :
            trainID(id), From(f), To(t),
            TimeL(l), TimeR(r),
            cost(cost_), time(tm), num(Num) {}

    void print() const {
        trainID.print();
        putchar(' ');
        From.print();
        putchar(' ');
        TimeL.print();
        printf(" -> ");
        To.print();
        putchar(' ');
        TimeR.print();
        printf(" %d %d\n", cost, num);
    }
};

struct Transfer_Ticket {
    Ticket A, B;
    int time, cost;

    Transfer_Ticket() {};

    Transfer_Ticket(int x) : time(x) {}

    Transfer_Ticket(const Ticket &a, const Ticket &b, const int &t) : A(a), B(b) {
        time = t + A.time + B.time;
        cost = A.cost + B.cost;
    }
};

bool Cmp1(const Transfer_Ticket &a, const Transfer_Ticket &b) {
    if (a.time != b.time)return a.time < b.time;
    if (a.cost != b.cost)return a.cost < b.cost;
    if (a.A.trainID != b.A.trainID)return a.A.trainID < b.A.trainID;
    return a.B.trainID < b.B.trainID;
}

bool Cmp2(const Transfer_Ticket &a, const Transfer_Ticket &b) {
    if (a.cost != b.cost)return a.cost < b.cost;
    if (a.time != b.time)return a.time < b.time;
    if (a.A.trainID != b.A.trainID)return a.A.trainID < b.A.trainID;
    return a.B.trainID < b.B.trainID;
}

bool cmp1(const Ticket2 &a, const Ticket2 &b) {
    return a.time < b.time || (a.time == b.time && a.trainID < b.trainID);
}

bool cmp2(const Ticket2 &a, const Ticket2 &b) {
    return a.cost < b.cost || (a.cost == b.cost && a.trainID < b.trainID);
}

typedef sjtu::pair<size_t, int> type1;

struct Pendtype{
    size_t username;
    int num,l,r,kth;
    Pendtype(){}

    Pendtype(const size_t &username_,const int &num_,const int &l_,const int &r_,const int &k_):
        username(username_),num(num_),l(l_),r(r_),kth(k_){}
};
