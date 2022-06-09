#ifndef SJTU_STRING_HPP
#define SJTU_STRING_HPP

#include <utility>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

namespace sjtu {

    class string {
    private:
        char alpha[65];
        int len = 0;
    public:
        string() {
            alpha[0] = '\0';
        }

        int size() const {
            return len;
        }
        
        void print()const{
        	printf("%s",alpha);
		}

        string(const string &other) {
            for (int i = 0; i < (int)other.size(); i++)
                alpha[i] = other.alpha[i];
            alpha[other.size()] = '\0';
            len = other.size();
        }
        
        std::string change() const{
			std::string ret;
			for(int i=0;i<len;i++)ret.push_back(alpha[i]);
			return ret;
		} 
        
        string(const std::string &other){
        	for(int i=0;i<(int)other.size();i++)alpha[i]=other[i];
        	alpha[other.size()]='\0';
        	len=other.size();
		}

        friend std::istream &operator>>(std::istream &is, string &s) {
            std::string s_;
            is >> s_;
            strcpy(s.alpha, s_.c_str());
            s.len = s_.size();
            return is;
        }
        
        friend std::ostream &operator<<(std::ostream &out,const string &s) {
			for(int i=0;i<s.len;i++)out<<s.alpha[i];  
            return out;
        }
        
        bool operator == (const string &s_)const {
            if (len != s_.size()) return 0;
            for (int i = 0; i < len; i++) {
                if (alpha[i] != s_[i]) return 0;
            }
            return 1;
        }
        
        bool operator != (const string &s_)const {
        	return !(*this==s_);
        }

        bool operator<(const string &s_) const {
            if (strcmp(alpha, s_.alpha) < 0) return 1;
            else return 0;
        }
        
        void operator = (const string &res){
        	len=res.len;
			strcpy(alpha,res.alpha);
		} 

        char &operator[](const int &i) {
            return alpha[i];
        }
        
		const char &operator[](const int &i)const  {
            return alpha[i];
        }
        

    };

}

#endif
