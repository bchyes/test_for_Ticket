#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

class token_scanner{
private:
	char buffer[10005];
	int point,sz;
	char delimiter;

public:
	token_scanner();
	
	~token_scanner();
	
	std::string next_token();//返回下一个字符串
	
	std::string all_token();
	
	void change(const char &c);
	
	bool read(){
		sz=0,point=0;
		static char c;
		while(c=getchar()){
			if(c==-1)return 0;
			if(c=='\n')break;
			buffer[sz++]=c;
		}
		buffer[sz]='\0';
		return 1;
	} 
};

token_scanner::token_scanner():sz(0),point(0),delimiter(' '){}

token_scanner::~token_scanner(){}

void token_scanner::change(const char &c){
	delimiter=c;
}

std::string token_scanner::next_token(){
	while(point<sz&&buffer[point]==delimiter)point++;
	if(point==sz)return "";
	int start=point;
	std::string ret;
	while(point<sz&&buffer[point]!=delimiter)point++;
	bool flag=point==sz&&buffer[point-1]=='\r';
	for(int i=start;i<point-flag;i++)ret.push_back(buffer[i]);
	return ret;
}

