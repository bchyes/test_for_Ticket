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

struct Basictrain {
    int maxday, maxseat;
    bool rel;

    Basictrain() {}

    Basictrain(const int &day, const int &seat, const int Rel = 0) : maxday(day), maxseat(seat), rel(Rel) {}
};

struct Stations {
    int pos, pri;
    Date ari, lef;

    Stations() {}

    Stations(const int &pos_, const int &pri_, const Date &ari_, const Date &lef_) : pos(pos_), pri(pri_), ari(ari_),
                                                                                     lef(lef_) {}
};

class train {
private:
    sjtu::string trainID;
    int stationNum, seatNum;
    sjtu::string stations[M];
    int prices[M], travelTimes[M], stopoverTimes[M];
    Date saleDateL, saleDateR;
    char Type;
    bool rel;
public:
    train() {}

    train(const sjtu::string &trainID_, const int &stationNum_, const sjtu::string *stations_, const int &seatNum_,
          const int *prices_, const int *travelTimes_,
          const int *stopoverTimes_, const Date &startTime_, const Date &saleDateL_, const Date &saleDateR_,
          const char &type_) : trainID(trainID_), stationNum(stationNum_), seatNum(seatNum_), rel(0) {
        saleDateL = Date(saleDateL_.m, saleDateL_.d, startTime_.hr, startTime_.mi);
        saleDateR = Date(saleDateR_.m, saleDateR_.d, startTime_.hr, startTime_.mi);
        Type = type_;
        for (int i = 0; i < stationNum; i++) {
            stations[i] = stations_[i];

            if (i < stationNum - 1) {
                travelTimes[i + 1] = travelTimes_[i];
                prices[i + 1] = prices_[i];
            }
        }
        stopoverTimes[0] = stopoverTimes[stationNum - 1] = prices[0] = travelTimes[0] = 0;
        for (int i = 1; i < stationNum - 1; i++)stopoverTimes[i] = stopoverTimes_[i - 1];
        for (int i = 1; i < stationNum; i++) {
            stopoverTimes[i] += stopoverTimes[i - 1];
            travelTimes[i] += travelTimes[i - 1];
            prices[i] += prices[i - 1];
        }
    }

    int query_time(int l, int r) const {
        return travelTimes[r] - travelTimes[l] + stopoverTimes[r - 1] - stopoverTimes[l];
    }

    inline Stations get_station(int rk) const {
        return Stations(rk, prices[rk], saleDateL + (travelTimes[rk] + stopoverTimes[std::max(rk - 1, 0)]),
                        saleDateL + (travelTimes[rk] + stopoverTimes[rk]));
    }

    friend TrainManagement;
};

class Left_Ticket {
private:
    int data[M];

public:
    Left_Ticket() {}

    Left_Ticket(const int &stationNum, const int &seat) {
        for (int j = 0; j < stationNum; j++)
            data[j] = seat;
    }

    inline void Update_ticket(const int &l, const int &r, const int &num) {
        for (int i = l; i < r; i++)data[i] -= num;
    }

    inline int query_ticket(const int &l, const int &r) const {
        int ret = 2e9;
        for (int i = l; i < r; i++)
            if (data[i] < ret)ret = data[i];
        return ret;
    }

    friend class TrainManagement;
};

class TrainManagement {
private:
    sjtu::linked_hashmap<size_t, Basictrain> bas;
    sjtu::linked_hashmap<size_t, sjtu::string> trainid;

    sjtu::bpt<size_t, train, 340, 4, 2> tra;
    sjtu::bpt<sjtu::pair<size_t, int>, Left_Ticket, 204, 9, 4> Tk;
    sjtu::bpt<type3, Stations, 204, 73, 4> sta;

    sjtu::bpt<type1, Ticket, 204, 25, 2> Get_ticket;//(username,kth) -> Ticket
    sjtu::bpt<sjtu::pair<type1, int>, Pendtype, 146, 85, 2> inqu_ticket;//((trainID,kth),timestamp) -> (username,Ticket)
    sjtu::bpt<size_t, int, 340, 255, 1> Num;//username -> tt

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


    int query_ticket(const sjtu::string &trainID_, const Date &L, const sjtu::string From_, const sjtu::string To_);

    void query_transfer(const std::string &From, const std::string &To, const Date &date,
                        bool flag);//query_transfer command

    void Update_ticket(const Ticket &t);

    void Reset();

    void query_ticket(const std::string &From, const std::string &To, const Date &date, bool flag);

    sjtu::pair<int, Ticket2>
    get_ticket2(const sjtu::string trainID_, const Stations &Lx, const Stations &Rx, const Date &date_);

    Ticket2 get_ticket(const sjtu::string &trainID_, const Stations &L, const Stations &R, const Date &date);

    void buy_ticket(const std::string &username, const std::string &trainID_, const Date &date, const std::string From_,
                    const std::string To_, const int &ask, const bool &flag, const int &timestamp);

    void Update_ticket(const size_t &trainID, const int &k, const int &l, const int &r, const int &num);

    int insert_ticket(const size_t &username_, const int &timestamp, const Ticket &t);

    void query_order(const size_t &username);

    void refund_ticket(const size_t &username, const int &k);

    void Print();

    double Size() {
        return (bas.Size() + trainid.size()) / 1024.0 / 1024.0 + tra.Size() + Tk.Size() + sta.Size() +
               Get_ticket.Size() + inqu_ticket.Size() + Num.Size();
    }
};

void TrainManagement::Print() {
    tra.print();
    Tk.print();
    sta.print();
    Get_ticket.print();
    inqu_ticket.print();
    Num.print();
}

int TrainManagement::query_ticket(const sjtu::string &trainID_, const Date &date, const sjtu::string From_,
                                  const sjtu::string To_) {
    size_t trainID = H(trainID_.change()), From = H(From_.change()), To = H(To_.change());
    auto L = sta.find(type3(From, trainID)), R = sta.find(type3(To, trainID));
    return Tk.find(type1(trainID, date - L.lef)).query_ticket(L.pos, R.pos);
}

TrainManagement::TrainManagement() : tra("file_train.dat", "file_train2.dat", "file_train_delete.dat",
                                         "file_train2_delete.dat"),
                                     sta("file_sta.dat", "file_sta2.dat", "file_sta_delete.dat",
                                         "file_sta2_delete.dat"),
                                     Tk("file_Ticket.dat", "file_Ticket2.dat", "file_Ticket_delete.dat",
                                        "file_Ticket2_delete.dat"),

                                     Get_ticket("file_get_ticket.dat", "file_get_ticket2.dat",
                                                "file_get_ticket_delete.dat", "file_get_ticket2_delete.dat"),
                                     inqu_ticket("file_inqu_ticket.dat", "file_inqu_ticket2.dat",
                                                 "file_inqu_ticket_delete.dat", "file_inqu_ticket2_delete.dat"),
                                     Num("file_num.dat", "file_num2.dat", "file_num_delete.dat",
                                         "file_num2_delete.dat") {
    if (tra.empty())return;
    const sjtu::vector<sjtu::pair<size_t, train> > &vec = tra.traverse_val(0, (size_t) -1);
    for (int i = 0; i < (int) vec.size(); i++) {
        const train &x = vec[i].second;
        bas[vec[i].first] = Basictrain(x.saleDateR - x.saleDateL, x.seatNum, x.rel);
        trainid[vec[i].first] = x.trainID;
    }
}

TrainManagement::~TrainManagement() {}

void TrainManagement::add_train(const std::string &trainID_, const int &stationNum_, const sjtu::string *stations_,
                                const int &seatNum, const int *prices_, const int *travelTimes_,
                                const int *stopoverTimes_, const Date &startTime_, const Date &saleDateL_,
                                const Date &saleDateR_, const char &type_) {
    size_t trainID = H(trainID_);
    if (bas.count(trainID))return void(puts("-1"));
    bas[trainID] = Basictrain(saleDateR_ - saleDateL_, seatNum);
    trainid[trainID] = sjtu::string(trainID_);
    tra.insert(trainID,
               train(sjtu::string(trainID_), stationNum_, stations_, seatNum, prices_, travelTimes_, stopoverTimes_,
                     startTime_, saleDateL_, saleDateR_, type_));
    for (int i = 0; i <= saleDateR_ - saleDateL_; i++)
        Tk.insert(type1(trainID, i), Left_Ticket(stationNum_, seatNum));
    puts("0");
}

void TrainManagement::delete_train(const std::string &trainID_) {
    size_t trainID = H(trainID_);
    if (!bas.count(trainID) || bas[trainID].rel == 1)return void(puts("-1"));
    tra.erase(trainID);
    for (int i = 0; i <= bas[trainID].maxday; i++) {
        Tk.erase(type1(trainID, i));
    }
    bas.erase(bas.find(trainID));
    trainid.erase(trainid.find(trainID));
    puts("0");
}

void TrainManagement::release_train(const std::string &trainID_, const int &timestamp) {
    size_t trainID = H(trainID_);
    if (!bas.count(trainID) || bas[trainID].rel == 1)return void(puts("-1"));
    train x = tra.find(trainID);
    for (int i = 0; i < x.stationNum; i++)
        sta.insert(type3(H(x.stations[i].change()), trainID), x.get_station(i));
    puts("0");
    x.rel = 1;
    tra.modify(trainID, x);
    bas[trainID].rel = 1;
}

void TrainManagement::query_train(const std::string &trainID_, const Date &date) {
    size_t trainID = H(trainID_);
    if (!bas.count(trainID))return void(puts("-1"));
    const train &x = tra.find(trainID);
    if (x.saleDateR < date || date < x.saleDateL)return void(puts("-1"));
    std::cout << trainID_;
    printf(" %c\n", x.Type);
    int k = date - x.saleDateL, cost = 0;
    const Left_Ticket &tk = Tk.find(type1(trainID, k));
    Date now(date.m, date.d, x.saleDateL.hr, x.saleDateL.mi);
    for (int i = 0; i < x.stationNum; i++) {
        x.stations[i].print();
        putchar(' ');
        if (i == 0) {
            printf("xx-xx xx:xx -> ");
            now.print();
            now += x.travelTimes[1];
        } else if (i == x.stationNum - 1) {
            cost += x.prices[i] - x.prices[i - 1];
            now.print();
            printf(" -> xx-xx xx:xx");
        } else {
            now.print();
            printf(" -> ");
            now += x.stopoverTimes[i] - x.stopoverTimes[i - 1];
            now.print();
            now += x.travelTimes[i + 1] - x.travelTimes[i];
            cost += x.prices[i] - x.prices[i - 1];
        }
        printf(" %d ", cost);
        if (i == x.stationNum - 1)puts("x");
        else printf("%d\n", tk.data[i]);
    }
}

inline Ticket2
TrainManagement::get_ticket(const sjtu::string &trainID_, const Stations &L, const Stations &R, const Date &date) {
    size_t trainID = H(trainID_.change());
    int k = date - L.lef;
    if (k < 0 || k > bas[trainID].maxday)return Ticket2();
    return Ticket2(trainID_, L.lef.ad_day(k), R.ari.ad_day(k), R.pri - L.pri,
                   Tk.find(type1(trainID, k)).query_ticket(L.pos, R.pos), R.ari.get_time(L.lef));
}

void TrainManagement::Update_ticket(const size_t &trainID, const int &k, const int &l, const int &r, const int &num) {
    auto now = Tk.find(type1(trainID, k));
    now.Update_ticket(l, r, num);
    Tk.modify(type1(trainID, k), now);
}

void TrainManagement::buy_ticket(const std::string &username, const std::string &trainID_, const Date &date,
                                 const std::string From_,
                                 const std::string To_, const int &ask, const bool &flag, const int &timestamp) {
    size_t trainID = H(trainID_), From = H(From_), To = H(To_);
    if (!bas.count(trainID) || bas[trainID].maxseat < ask)return void(puts("-1"));
    auto L = sta.find2(type3(From, trainID)), R = sta.find2(type3(To, trainID));
    if (L.first == 0 || R.first == 0 || L.second.pos > R.second.pos)return void(puts("-1"));
    Ticket2 tk = get_ticket(sjtu::string(trainID_), L.second, R.second, date);
    if (tk.cost == -1)return void(puts("-1"));//时间超
    if (flag == 0 && tk.num < ask)return void(puts("-1"));//不候补
    int k = date - L.second.lef;
    if (tk.num >= ask) {
        tk.num = ask;
        Update_ticket(trainID, k, L.second.pos, R.second.pos, ask);
        insert_ticket(H(username), timestamp, Ticket(tk, From_, To_, timestamp));
        printf("%lld\n", 1LL * ask * tk.cost);
    } else {
        tk.num = ask;
        int id = insert_ticket(H(username), timestamp, Ticket(tk, From_, To_, -timestamp));
        inqu_ticket.insert(sjtu::pair<type1, int>(type1(trainID, k), timestamp),
                           Pendtype(H(username), ask, L.second.pos, R.second.pos, id));
        puts("queue");
    }

}

void TrainManagement::Update_ticket(const Ticket &t) {
    size_t trainID = H(t.trainID.change()), From = H(t.From.change()), To = H(t.To.change());
    auto L = sta.find(type3(From, trainID)), R = sta.find(type3(To, trainID));
    auto now = Tk.find(type1(trainID, t.TimeL - L.lef));
    now.Update_ticket(L.pos, R.pos, t.num);
    Tk.modify(type1(trainID, t.TimeL - L.lef), now);
}

Ticket2 A[M * M], tmp[M * M];
int ticket_num;

bool (*cmp)(const Ticket2 &a, const Ticket2 &b);

bool (*Cmp)(const Transfer_Ticket &a, const Transfer_Ticket &b);

void sort(int l, int r) {
    if (l >= r)return;
    int mid = l + r >> 1;
    sort(l, mid), sort(mid + 1, r);
    int i = l, j = mid + 1, k = l;
    while (i <= mid && j <= r)
        if (cmp(A[i], A[j]))tmp[k++] = A[i++];
        else tmp[k++] = A[j++];
    while (i <= mid)tmp[k++] = A[i++];
    while (j <= r)tmp[k++] = A[j++];
    for (i = l; i <= r; i++)A[i] = tmp[i];
}

void TrainManagement::query_ticket(const std::string &From, const std::string &To, const Date &date, bool flag) {
    if (sta.empty())return void(puts("0"));
    const sjtu::vector<sjtu::pair<type3, Stations> > &vec1 = sta.traverse_val(type3(H(From), 0),
                                                                              type3(H(From), (size_t) -1));
    const sjtu::vector<sjtu::pair<type3, Stations> > &vec2 = sta.traverse_val(type3(H(To), 0),
                                                                              type3(H(To), (size_t) -1));
    if (vec1.size() == 0 || vec2.size() == 0)return void(puts("0"));
    ticket_num = 0;
    for (int i = 0, j = 0, sz = vec2.size(); i < (int) vec1.size(); i++) {
        while (j + 1 < sz && vec2[j].first.second < vec1[i].first.second)j++;
        if (vec2[j].first.second == vec1[i].first.second) {
            const Stations &L = vec1[i].second, &R = vec2[j].second;
            if (L.pos < R.pos) {
                size_t trainID = vec1[i].first.second;
                const Ticket2 &tik = get_ticket(trainid[trainID], L, R, date);
                if (tik.cost != -1)A[++ticket_num] = tik;
            }
        }
    }

    cmp = flag == 0 ? cmp1 : cmp2;
    sort(1, ticket_num);

    printf("%d\n", ticket_num);
    for (int i = 1; i <= ticket_num; i++)A[i].print(From, To);
}

sjtu::pair<int, Ticket2>
TrainManagement::get_ticket2(const sjtu::string trainID_, const Stations &Lx, const Stations &Rx, const Date &date_) {
    size_t trainID = H(trainID_.change());
    if (Lx.pos > Rx.pos)return sjtu::pair<int, Ticket2>(-1, Ticket2());
    Date date(date_);
    Date L = Lx.lef, R = Lx.lef.ad_day(bas[trainID].maxday);

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
    int mi = Rx.ari.get_time(Lx.lef);
    date.hr = L.hr, date.mi = L.mi;
    return sjtu::pair<int, Ticket2>(Ret, Ticket2(trainID_, date, date + mi, Rx.pri - Lx.pri,
                                                 Tk.find(type1(trainID, date - L)).query_ticket(Lx.pos, Rx.pos), mi));
}

void TrainManagement::query_transfer(const std::string &From, const std::string &To, const Date &date, bool flag) {

    if (sta.empty())return void(puts("0"));
    sjtu::vector<sjtu::pair<type3, Stations> > vec1 = sta.traverse_val(type3(H(From), 0), type3(H(From), (size_t) -1));
    sjtu::vector<sjtu::pair<type3, Stations> > vec2 = sta.traverse_val(type3(H(To), 0), type3(H(To), (size_t) -1));

    bool First = 1;
    Cmp = flag == 0 ? Cmp1 : Cmp2;
    Transfer_Ticket ret, TMP;
    sjtu::linked_hashmap<size_t, train> T;


    for (int i = 0; i < (int) vec1.size(); i++) {
        const Stations &L = vec1[i].second;
        if (!T.count(vec1[i].first.second))T[vec1[i].first.second] = tra.find(vec1[i].first.second);
        const train &now = T[vec1[i].first.second];
        const sjtu::string &trainID = now.trainID;
        sjtu::linked_hashmap<size_t, int> hv;

        for (int j = L.pos + 1; j < now.stationNum; j++)hv[H(now.stations[j].change())] = j;

        for (int j = 0; j < (int) vec2.size(); j++) {
            if (vec1[i].first.second == vec2[j].first.second)continue;
            if (!T.count(vec2[j].first.second))T[vec2[j].first.second] = tra.find(vec2[j].first.second);

            const train &to = T[vec2[j].first.second];

            for (int k = 0, ed = vec2[j].second.pos; k < ed; k++) {
                size_t id = H(to.stations[k].change());
                if (!hv.count(id))continue;
                const Ticket2 &A = get_ticket(trainID, L, now.get_station(hv[id]), date);
                if (A.cost == -1)continue;
                const sjtu::pair<int, Ticket2> &B = get_ticket2(to.trainID, to.get_station(k), vec2[j].second, A.TimeR);
                if (B.first != -1) {
                    TMP = Transfer_Ticket(A, B.second, B.first, From, to.stations[k].change(), To);
                    if (First)ret = TMP, First = 0;
                    else if (Cmp(TMP, ret))ret = TMP;
                }
            }

        }
    }
    //std::cerr<<T.Size()/1024.0/1024.0<<std::endl;

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

int TrainManagement::insert_ticket(const size_t &username, const int &timestamp, const Ticket &t) {
    if (Num.empty())Num.insert(username, 0);
    auto now = Num.find2(username);
    if (now.first == 0) {
        Num.insert(username, 0);
        now = Num.find2(username);
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
        if (vec[i].time > 0)printf("[success] ");
        else if (vec[i].time < 0)printf("[pending] ");
        else printf("[refunded] ");
        vec[i].print();
    }
}

void TrainManagement::refund_ticket(const size_t &username, const int &k) {
    if (Num.empty())return void(puts("-1"));
    auto tmp = Num.find2(username);
    if (tmp.first == 0 || k > tmp.second)return void(puts("-1"));
    int tt = tmp.second;
    Ticket t = Get_ticket.find(type1(username, tt - k + 1));
    if (t.time == 0)return void(puts("-1"));
    size_t trainID = H(t.trainID.change());
    auto Sta = sta.find(type3(H(t.From.change()), trainID));
    if (t.time < 0) {
        inqu_ticket.erase(sjtu::pair<type1, int>(type1(trainID, t.TimeL - Sta.lef), -t.time));
        t.time = 0;
        Get_ticket.modify(type1(username, tt - k + 1), t);
    } else {
        t.time = 0;
        Get_ticket.modify(type1(username, tt - k + 1), t);
        t.num = -t.num, Update_ticket(t);
        const int &K = t.TimeL - Sta.lef;
        const type1 res(trainID, K);
        if (!inqu_ticket.empty()) {
            const sjtu::vector<sjtu::pair<sjtu::pair<type1, int>, Pendtype> > &vec = inqu_ticket.traverse_val(
                    sjtu::pair<type1, int>(res, 0), sjtu::pair<type1, int>(res, 2e9));
            Left_Ticket tk = Tk.find(type1(trainID, K));
            for (int i = 0; i < (int) vec.size(); i++) {
                const Pendtype &x = vec[i].second;
                if (tk.query_ticket(x.l, x.r) >= x.num) {
                    tk.Update_ticket(x.l, x.r, x.num);
                    auto us = Get_ticket.find(type1(x.username, x.kth));
                    us.time = -us.time;
                    Get_ticket.modify(type1(x.username, x.kth), us);
                    inqu_ticket.erase(vec[i].first);
                }
            }
            Tk.modify(type1(trainID, K), tk);
        }
    }
    puts("0");
}
