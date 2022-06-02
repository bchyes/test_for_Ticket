#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

class token_scanner{
private:
	std::string buffer;
	int point;
	char delimiter;

public:
	token_scanner();
	
	token_scanner(const std::string &_buffer,const int &_point,const char &_delimiter);
	
	~token_scanner();
	
	std::string next_token();//返回下一个字符串
	
	std::string all_token();
	
	void change(const char &c);
	
	friend std::istream& operator>>(std::istream& input, token_scanner& obj);//读入新的字符串 
};

token_scanner::token_scanner():buffer(""),point(0),delimiter(' '){}

token_scanner::token_scanner(const std::string &_buffer,const int &_point,const char &_delimiter):
	buffer(_buffer),point(_point),delimiter(_delimiter){}

token_scanner::~token_scanner(){}

void token_scanner::change(const char &c){
	delimiter=c;
}

std::string token_scanner::next_token(){
	while(point<buffer.size()&&buffer[point]==delimiter)point++;
	if(point==buffer.size())return "";
	int start=point;
	while(point<buffer.size()&&buffer[point]!=delimiter)point++;
	bool flag=point==buffer.size()&&buffer[point-1]=='\r';
	return buffer.substr(start,point-start-flag);
}

std::string token_scanner::all_token(){
	return buffer;
}

std::istream& operator>>(std::istream& input, token_scanner& obj){
	std::string line;
	getline(input,line);
	obj=token_scanner(line,0,' ');
	return input;
}
