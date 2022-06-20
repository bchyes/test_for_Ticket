#ifndef SJTU_STACK_HPP
#define SJTU_STACK_HPP

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>
namespace sjtu {

template<class Key>
class Stack {
private:
    int sz,Size;
    std::string file_name;
    std::fstream file;

    public:
        explicit Stack(const std::string &file_name_) : file_name(file_name_) {
            Size = sizeof(Key);
            file.open(file_name);
            if (!file.is_open()) {
                file.open(file_name, std::ios::out);
                sz = 0;
                file.write(reinterpret_cast<char *>(&sz), 4);
            } else file.read(reinterpret_cast<char *>(&sz), 4);
            file.close();
        }

        bool empty(){
            return sz==0;
        }

        void push(Key key) {
            file.open(file_name);
            file.seekp(4 + 1LL*sz * Size, std::ios::beg);
            file.write(reinterpret_cast<char * >(&key), Size);
            sz++;
            file.close();
        }

        Key top() {
            file.open(file_name);
            file.seekp(4 + 1LL*(sz-1) * Size, std::ios::beg);
            Key ret;
            file.read(reinterpret_cast<char *>(&ret), Size);
            file.close();
            return ret;
        }

        void pop() {
            sz--;
        }

        ~Stack() {
            file.open(file_name);
            file.write(reinterpret_cast<char *>(&sz), 4);
            file.close();
        }

        int size(){
            return sz;
        }
    };

}

#endif