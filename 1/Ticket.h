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

struct Ticket {
    sjtu::string trainID, From, To;
    Date TimeL, TimeR;
    int cost, num, time;

    Ticket() : cost(0), num(1e9), time(0) {}

    Ticket(const sjtu::string &res) : trainID(res) {}

    Ticket(const std::string &res) : trainID(res) {}

    Ticket(const Ticket &res) :
            trainID(res.trainID), From(res.From), To(res.To),
            TimeL(res.TimeL), TimeR(res.TimeR),
            cost(res.cost), time(res.time), num(res.num) {}

    void print() {
        std::cout << trainID << ' ' << From << ' ';
        TimeL.print();
        std::cout << " -> ";
        std::cout << To << ' ';
        TimeR.print();
        std::cout << ' ' << cost << ' ' << num << std::endl;
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

bool cmp1(const Ticket &a, const Ticket &b) {
    return a.time < b.time || a.time == b.time && a.trainID < b.trainID;
}

bool cmp2(const Ticket &a, const Ticket &b) {
    return a.cost < b.cost || a.cost == b.cost && a.trainID < b.trainID;
}

typedef sjtu::pair<sjtu::string, int> type1;
typedef sjtu::pair<sjtu::string, Ticket> type2;


class TicketManagement {
private:
    sjtu::bpt<type1, Ticket> get_ticket;//(username,timestamp) -> Ticket
    sjtu::bpt<type1, type2> inqu_ticket;//(trainID,timestamp) -> (username,Ticket)

public:
    TicketManagement();

    ~TicketManagement();

    void insert_ticket(const sjtu::string &username, const int &timestamp, const Ticket &t, const bool flag);

    void query_order(const sjtu::string &username);

    Ticket refund_ticket(const sjtu::string &username, const int &k);

    type2 next_ticket(const sjtu::string &username, int &kth);

    void Update_ticket(const sjtu::string &username, const Ticket &t, const int &timestamp);

    void Reset();

};

TicketManagement::TicketManagement() : get_ticket("file_get_ticket", "file_get_ticket_delete"),
                                       inqu_ticket("file_inqu_ticket", "file_inqu_ticket_delete") {}

TicketManagement::~TicketManagement() {};

void
TicketManagement::insert_ticket(const sjtu::string &username, const int &timestamp, const Ticket &t, const bool flag) {
    Ticket tmp = t;
    if (flag == 0)tmp.time = 1;
    else tmp.time = 2;
    get_ticket.insert(sjtu::pair<type1, Ticket>(type1(username, timestamp), tmp));
    if (tmp.time == 2)
        inqu_ticket.insert(sjtu::pair<type1, type2>(type1(t.trainID, timestamp), type2(username, tmp)));
}

void TicketManagement::query_order(const sjtu::string &username_) {
    sjtu::string username(username_);
    sjtu::vector<Ticket> vec;
    type1 last(username, 0), now = get_ticket.upper_bound(last);
    while (now.first == username && last != now) {
        vec.push_back(get_ticket.find(now));
        last = now;
        now = get_ticket.upper_bound(last);

    }
    std::cout << vec.size() << std::endl;
    for (int i = (int) vec.size() - 1; i >= 0; i--) {
        if (vec[i].time == 1)std::cout << "[success] ";
        else if (vec[i].time == 2)std::cout << "[pending] ";
        else std::cout << "[refunded] ";
        vec[i].print();
    }
}

Ticket TicketManagement::refund_ticket(const sjtu::string &username, const int &k) {
    sjtu::vector<Ticket> vec;
    sjtu::vector<int> Vec;
    type1 last(username, 0), now = get_ticket.upper_bound(last);
    while (now.first == username && last != now) {
        vec.push_back(get_ticket.find(now));
        Vec.push_back(now.second);
        last = now;
        now = get_ticket.upper_bound(last);
    }
    if (k > vec.size())return Ticket(sjtu::string(""));
    Ticket t = vec[vec.size() - k];
    if (t.time == 3)return Ticket(sjtu::string(""));
    int last_ = t.time;
    t.time = 3;
    get_ticket.modify(type1(username, Vec[Vec.size() - k]), t);
    t.time = last_;
    return t;
}

type2 TicketManagement::next_ticket(const sjtu::string &trainID, int &k) {
    if (inqu_ticket.empty())return type2("", "");
    type1 tmp = type1(trainID, k), now = inqu_ticket.upper_bound(tmp);
    if (tmp == now || !(now.first == tmp.first))return type2("", "");
    return k = now.second, inqu_ticket.find(now);
}

void TicketManagement::Update_ticket(const sjtu::string &username, const Ticket &t, const int &timestamp) {
    inqu_ticket.erase(type1(t.trainID, timestamp));
    Ticket tmp = get_ticket.find(type1(username, timestamp));
    tmp.time = 1;
    get_ticket.modify(type1(username, timestamp), tmp);
}

void TicketManagement::Reset() {
    get_ticket.clean();
    inqu_ticket.clean();
}
