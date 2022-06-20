#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

const int DAY[]={0,1,2,3,4,5,30,31,31,30,31,30,31};
struct Date{
	char m,d,hr,mi;
	
	inline int change(int fl)const {
		if(fl==0)return (int)(24-hr)*60-mi;
		return (int)hr*60+mi;
	}
	
	inline bool Compare(const Date &res)const {
		return hr<res.hr||(hr==res.hr&&mi<=res.mi);
	}
	
	inline int del(const Date &res)const{
		return (int)(hr-res.hr)*60+mi-res.mi;
	}

	
	inline void print()const{
		printf("0%d-%02d %02d:%02d",m,d,hr,mi);
	} 
	
	void operator += (const int &t){
		int Mi=mi+t,Hr=hr+Mi/60;
		mi=Mi%60;
		hr=Hr%24;
		d+=Hr/24;
		while(DAY[m]<d)d-=DAY[m++];
	}
	
	Date operator + (const int &x)const{
		Date ret;
		int Mi=mi+x,Hr=hr+Mi/60;
		ret.mi=Mi%60;
		ret.d=d+Hr/24;
		ret.hr=Hr%24;
		ret.m=m;
		while(DAY[ret.m]<ret.d)ret.d-=DAY[ret.m++];
		return ret; 
	}
	
	bool operator < (const Date &res)const{
		return m<res.m|| (m==res.m&&d<res.d);
	}
	
	Date():m(0),d(0),hr(0),mi(0){};
	
	Date(const std::string &s){
		m=d=hr=mi=0; 
		if(s[2]=='-')m=s[1]-'0',d=(s[3]-'0')*10+s[4]-'0';
		else hr=(s[0]-'0')*10+s[1]-'0',mi=(s[3]-'0')*10+s[4]-'0';
	}
	
	Date(const int &m_,const int &d_,const int &hr_,const int &mi_):
		m(m_),d(d_),hr(hr_),mi(mi_){}
	
	Date(const Date &res):
		m(res.m),d(res.d),hr(res.hr),mi(res.mi){}
	
	int operator - (const Date &res)const{
		int ret=0;
		if(m==res.m)return d-res.d;
		else if(m<res.m)return -233;
		int D=res.d,M=res.m;
		while(M<m){
			ret+=DAY[M++]-D+1;
			D=1;
		}
		return ret+d-D;
	}

	inline int get_time(const Date &res) const {
		return del(res)+((*this)-res)*1440;
	}
	
	inline Date ad_day(const int &x)const {
		int D=d+x,M=m;
		while(D>DAY[M])D-=DAY[M++];
		return Date(M,D,hr,mi);
	}
};
