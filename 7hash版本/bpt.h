#ifndef MAIN_MAP_CPP_BPT_H
#define MAIN_MAP_CPP_BPT_H

#include <iostream>
#include <functional>
#include <cstddef>
#include <fstream>
#include <cstring>
#include "vector.hpp"
#include "utility.hpp"
#include "exceptions.hpp"
#include "linked_hashmap.hpp"
#include "map.hpp"

namespace sjtu {
    template<class Key, class T, int M = 30, class Compare= std::less<Key> >
    class bpt {
    private:
        typedef pair<Key, T> value_type;
        struct node {
            bool is_leave = 0;
            int length = 0;
            long long address = 0;
            long long father = -1;
            value_type value[M];
            long long son[M];
            long long pre = -1;
            long long next = -1;
        };
        node root;
        node node_back;
        node tmp_;
        node now;
        node tmp;
        node tmp_0;
        node new_tmp;
        node ept;
        //sjtu::linked_hashmap<Key, T> hs;
        sjtu::map<Key, T> hs;
        std::fstream file;
        std::fstream file_delete;
        std::string file_name;
        std::string file_delete_name;
        Compare cpy;
        sjtu::vector<long long> delete_;
        int delete_num = 0;

        void split(node &tmp, node &now) {
            value_type v_up;
            tmp.length = M >> 1;
            if (!delete_num) {
                file.seekp(0, std::fstream::end);
            } else {
                long long x = delete_.back();
                delete_.pop_back();
                delete_num--;
                file.seekg(x);
            }
            //node new_tmp;
            new_tmp = ept;//置空
            new_tmp.address = file.tellp();//新取一个地址 可记录
            new_tmp.father = now.address;
            if (tmp.next != -1) {//先将叶子节点的链表连起来
                file.seekg(tmp.next);
                //node tmp_0;
                file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                tmp_0.pre = new_tmp.address;
                new_tmp.next = tmp_0.address;
                file.seekp(tmp_0.address);
                file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
            }
            tmp.next = new_tmp.address;
            new_tmp.pre = tmp.address;
            file.seekp(tmp.address);
            file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
            for (int i = 0; i < M - tmp.length; i++)//将原来tmp中的值拿一半到new_tmp中
                new_tmp.value[i] = tmp.value[i + tmp.length];
            new_tmp.length = M - tmp.length;
            new_tmp.is_leave = 1;
            file.seekp(new_tmp.address);
            file.write(reinterpret_cast<char *>(&new_tmp), sizeof(node));
            v_up = new_tmp.value[0];//记录用来改变非叶节点的索引
            bool root_ = 1;//判断需不需要对根进行操作，用于下方第二个while，若根的儿子有分裂，则需要进行操作，初始设为1为了处理根的儿子是叶子节点的情况
            node_back = new_tmp;//!
            while (node_back.father) {//如果新添加的节点new_tmp有父亲，则对他的父亲的索引进行修改。
                root_ = 0;
                file.seekg(node_back.father);
                file.read(reinterpret_cast<char *>(&node_back), sizeof(node));
                int i;
                for (i = 0; i < node_back.length - 1; i++) {
                    if (cpy(new_tmp.value[0].first, node_back.value[i].first)) {
                        for (int j = node_back.length - 1; j >= i; j--) {
                            node_back.value[j + 1] = node_back.value[j];
                            node_back.son[j + 1] = node_back.son[j];
                        }
                        node_back.value[i] = v_up;
                        node_back.son[i + 1] = new_tmp.address;//!
                        break;
                    }
                }
                if (i + 1 == node_back.length) {
                    node_back.value[node_back.length - 1] = v_up;
                    node_back.son[node_back.length] = new_tmp.address;
                }
                node_back.length++;
                file.seekp(node_back.address);
                file.write(reinterpret_cast<char *>(&node_back), sizeof(node));
                if (node_back.length == M) {//如果父亲仍然需要分裂，则同上述操作（和叶节点有点不同）
                    if (!node_back.father) root_ = 1;//此处判断需不需要对根节点操作
                    node_back.length = M >> 1;
                    file.seekp(node_back.address);
                    file.write(reinterpret_cast<char *>(&node_back), sizeof(node));
                    for (int i = 0; i < M - node_back.length - 1; i++) {
                        new_tmp.value[i] = node_back.value[i + node_back.length];
                        new_tmp.son[i] = node_back.son[i + node_back.length];
                    }
                    v_up = node_back.value[node_back.length - 1];
                    new_tmp.length = M - node_back.length;
                    new_tmp.son[new_tmp.length - 1] = node_back.son[M - 1];
                    if (!delete_num) {
                        file.seekp(0, std::fstream::end);
                    } else {
                        long long x = delete_.back();
                        delete_.pop_back();
                        delete_num--;
                        file.seekg(x);
                    }
                    new_tmp.address = file.tellg();//新取一个地址 可记录
                    new_tmp.father = node_back.father;//!!
                    new_tmp.is_leave = 0;
                    file.write(reinterpret_cast<char *>(&new_tmp), sizeof(node));
                    //node tmp_;
                    for (int i = 0; i < new_tmp.length; i++) {
                        file.seekg(new_tmp.son[i]);
                        file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                        tmp_.father = new_tmp.address;
                        file.seekp(new_tmp.son[i]);
                        file.write(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                    }
                } else break;
            }
            if (!node_back.father && root_) {//对根节点进行操作
                file.seekg(node_back.father);
                file.read(reinterpret_cast<char *>(&node_back), sizeof(node));
                int i;
                for (i = 0; i < node_back.length - 1; i++) {
                    if (cpy(new_tmp.value[0].first, node_back.value[i].first)) {
                        for (int j = node_back.length - 1; j >= i; j--) {
                            node_back.value[j + 1] = node_back.value[j];//第一个无用移动
                            node_back.son[j + 1] = node_back.son[j];//最后一个无效移动
                        }
                        node_back.value[i] = v_up;
                        node_back.son[i + 1] = new_tmp.address;//!
                        break;
                    }
                }
                if (i + 1 == node_back.length) {
                    node_back.value[node_back.length - 1] = v_up;
                    node_back.son[node_back.length] = new_tmp.address;
                }
                node_back.length++;
                file.seekp(node_back.address);
                file.write(reinterpret_cast<char *>(&node_back), sizeof(node));
                if (node_back.length == M) {
                    node_back.length = M >> 1;
                    file.seekp(node_back.address);
                    file.write(reinterpret_cast<char *>(&node_back), sizeof(node));
                    for (int i = 0; i < M - node_back.length - 1; i++) {
                        new_tmp.value[i] = node_back.value[i + node_back.length];
                        new_tmp.son[i] = node_back.son[i + node_back.length];
                    }
                    v_up = node_back.value[node_back.length - 1];
                    new_tmp.length = M - node_back.length;
                    new_tmp.son[new_tmp.length - 1] = node_back.son[M - 1];
                    if (!delete_num) {
                        file.seekp(0, std::fstream::end);
                    } else {
                        long long x = delete_.back();
                        delete_.pop_back();
                        delete_num--;
                        file.seekg(x);
                    }
                    new_tmp.address = file.tellg();//新取一个地址 可记录
                    new_tmp.father = 0;
                    new_tmp.is_leave = 0;
                    file.write(reinterpret_cast<char *>(&new_tmp), sizeof(node));
                    //node tmp_;
                    for (int i = 0; i < new_tmp.length; i++) {
                        file.seekg(new_tmp.son[i]);
                        file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                        tmp_.father = new_tmp.address;
                        file.seekp(new_tmp.son[i]);
                        file.write(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                    }
                    if (!delete_num) {
                        file.seekp(0, std::fstream::end);
                    } else {
                        long long x = delete_.back();
                        delete_.pop_back();
                        delete_num--;
                        file.seekg(x);
                    }
                    node_back.address = file.tellg();//新取一个地址 可记录
                    node_back.father = 0;
                    file.write(reinterpret_cast<char *>(&node_back), sizeof(node));
                    for (int i = 0; i < node_back.length; i++) {
                        file.seekg(node_back.son[i]);
                        file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                        tmp_.father = node_back.address;
                        file.seekp(node_back.son[i]);
                        file.write(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                    }
                    //需要提供一个新的根把原本根分裂出来的节点作为他的儿子
                    root.length = 2;
                    root.son[0] = node_back.address;
                    root.son[1] = new_tmp.address;
                    root.value[0] = v_up;
                    file.seekp(0, std::fstream::beg);
                    file.write(reinterpret_cast<char *>(&root), sizeof(node));
                }
            }
        }

        void insert_leave(const value_type &value, node &tmp, node &now) {
            int i;
            for (i = 0; i < tmp.length; i++) {//添加节点
                if (!cpy(value.first, tmp.value[i].first) && !cpy(tmp.value[i].first, value.first)) {
                    file.close();
                    throw int();
                }
                if (cpy(value.first, tmp.value[i].first)) {
                    for (int j = tmp.length - 1; j >= i; j--)//往后移动
                        tmp.value[j + 1] = tmp.value[j];
                    tmp.value[i] = value;
                    break;
                }
            }
            if (i == tmp.length)
                tmp.value[tmp.length] = value;
            tmp.length++;
            file.seekp(tmp.address);
            file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
            if (tmp.length == M) {//如果叶节点个数过多则需要分裂
                split(tmp, now);
            }
        }

        void merge(node &now, value_type &v_up) {
            //node tmp;
            if (now.pre != -1) {//如果合并的节点左边有节点的话
                file.seekg(now.pre);
                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                if (tmp.length > (M) / 2) {//如果左边的节点可以移动一个节点过来
                    now.length++;
                    tmp.length--;
                    for (int k = now.length - 1; k >= 0; k--)
                        now.value[k + 1] = now.value[k];
                    now.value[0] = tmp.value[tmp.length];
                    file.seekp(tmp.address);
                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                    file.seekp(now.address);
                    file.write(reinterpret_cast<char *>(&now), sizeof(node));
                    file.seekg(now.father);
                    file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                    bool ok = 0;
                    while (1) {//修改祖先上索引的值
                        for (int k = 0; k < tmp.length - 1; k++) {
                            if (!cpy(tmp.value[k].first, v_up.first) &&
                                !cpy(v_up.first, tmp.value[k].first)) {
                                tmp.value[k] = now.value[0];
                                file.seekp(tmp.address);
                                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                ok = 1;
                                break;
                            }
                        }
                        if (ok) break;
                        else {
                            file.seekg(tmp.father);
                            file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                        }
                    }
                    file.close();
                    return;
                }
            }
            if (now.next != -1) {//同理判断右边的节点
                file.seekg(now.next);
                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                if (tmp.length > (M) / 2) {
                    now.length++;
                    tmp.length--;
                    now.value[now.length - 1] = tmp.value[0];
                    value_type up = tmp.value[0];
                    for (int k = 0; k <= tmp.length - 1; k++)
                        tmp.value[k] = tmp.value[k + 1];
                    file.seekp(tmp.address);
                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                    file.seekp(now.address);
                    file.write(reinterpret_cast<char *>(&now), sizeof(node));
                    file.seekg(now.father);
                    file.read(reinterpret_cast<char *>(&now), sizeof(node));
                    bool ok = 0;
                    while (1) {
                        for (int k = 0; k < now.length - 1; k++) {
                            if (!cpy(now.value[k].first, up.first) &&
                                !cpy(up.first, now.value[k].first)) {
                                now.value[k] = tmp.value[0];
                                file.seekp(now.address);
                                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                ok = 1;
                                break;
                            }
                        }
                        if (ok) break;
                        else {
                            file.seekg(now.father);
                            file.read(reinterpret_cast<char *>(&now), sizeof(node));
                        }
                    }
                    file.close();
                    return;
                }
            }
            if (now.pre != -1 || now.next != -1) {//以下判断的是需要合并的情况
                if (now.pre != -1) {//如果左边有节点，由上述判断知道肯定只能合并
                    file.seekg(now.pre);
                    file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                    if (now.father == tmp.father) {//如果两个节点父亲是同一个节点，否则就考虑右边的节点，此时右边的节点肯定是同一个父亲
                        delete_num++;
                        delete_.push_back(now.address);//!删除
                        for (int k = 0; k < now.length; k++) {
                            tmp.value[k + tmp.length] = now.value[k];
                        }
                        tmp.length += now.length;
                        tmp.next = now.next;
                        file.seekp(tmp.address);
                        file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                        if (tmp.next != -1) {
                            file.seekg(tmp.next);
                            //node tmp_;
                            file.read(reinterpret_cast<char *>(&tmp_), sizeof(node));
                            tmp_.pre = tmp.address;
                            file.seekp(tmp_.address);
                            file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                        }
                        file.seekg(now.father);
                        file.read(reinterpret_cast<char *>(&now), sizeof(node));
                        for (int k = 0; k < now.length - 1; k++) {
                            if (!cpy(v_up.first, now.value[k].first) &&
                                !cpy(now.value[k].first, v_up.first)) {
                                for (int u = k; u < now.length - 2; u++) {
                                    now.value[u] = now.value[u + 1];
                                    now.son[u + 1] = now.son[u + 2];
                                }
                                break;
                            }
                        }
                        now.length--;
                        file.seekp(now.address);
                        file.write(reinterpret_cast<char *>(&now), sizeof(node));
                        //bool root_ = 0;
                        while (now.father != -1) {//寻找祖先进行合并
                            if (now.length <= (M - 2) / 2) {//如果这个点需要合并，则执行和上面一样的操作，否则直接break
                                file.seekg(now.father);
                                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                                int k;
                                for (k = 0; k < tmp.length - 1; k++) {
                                    if (cpy(now.value[0].first, tmp.value[k].first))
                                        break;
                                }
                                if (k != tmp.length - 1) {//如果不是最后一个
                                    file.seekg(tmp.son[k + 1]);
                                    //node tmp_;
                                    file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                    if (tmp_.length > (M) / 2) {//如果可以挪一个数据过来
                                        now.length++;
                                        now.value[now.length - 2] = tmp.value[k];
                                        now.son[now.length - 1] = tmp_.son[0];
                                        file.seekg(now.son[now.length - 1]);
                                        //node tmp_0;
                                        file.read(reinterpret_cast<char *>(&tmp_0), sizeof(tmp_0));
                                        tmp_0.father = now.address;
                                        file.seekp(now.son[now.length - 1]);
                                        file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                        tmp.value[k] = tmp_.value[0];
                                        for (int u = 0; u < tmp_.length - 2; u++) {
                                            tmp_.value[u] = tmp_.value[u + 1];
                                            tmp_.son[u] = tmp_.son[u + 1];
                                        }
                                        tmp_.son[tmp_.length - 2] = tmp_.son[tmp_.length - 1];
                                        tmp_.length--;
                                        file.seekp(now.address);
                                        file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                        file.seekp(tmp.address);
                                        file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                        file.seekp(tmp_.address);
                                        file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                        break;
                                    } else {
                                        delete_num++;
                                        delete_.push_back(tmp_.address);//!删除
                                        now.value[now.length - 1] = tmp.value[k];
                                        for (int u = 0; u < tmp_.length - 1; u++) {
                                            now.value[u + now.length] = tmp_.value[u];
                                            now.son[u + now.length] = tmp_.son[u];
                                        }
                                        now.length += tmp_.length;
                                        now.son[now.length - 1] = tmp_.son[tmp_.length - 1];
                                        file.seekp(now.address);
                                        file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                        for (int u = 0; u <= tmp_.length - 1; u++) {
                                            file.seekg(now.son[u + now.length - tmp_.length]);
                                            //node tmp_0;
                                            file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                            tmp_0.father = now.address;
                                            file.seekp(now.son[u + now.length - tmp_.length]);
                                            file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                        }
                                        for (int u = k; u < tmp.length - 2; u++) {
                                            tmp.value[u] = tmp.value[u + 1];
                                            tmp.son[u + 1] = tmp.son[u + 2];
                                        }
                                        tmp.length--;
                                        file.seekp(tmp.address);
                                        file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                    }
                                } else {
                                    file.seekg(tmp.son[k - 1]);
                                    //node tmp_;
                                    file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                    if (tmp_.length > (M) / 2) {
                                        now.son[now.length] = now.son[now.length - 1];
                                        for (int u = now.length - 1; u >= 1; u--) {
                                            now.son[u] = now.son[u - 1];
                                            now.value[u] = now.value[u - 1];
                                        }
                                        ++now.length;
                                        now.value[0] = tmp.value[tmp.length - 2];
                                        now.son[0] = tmp_.son[tmp_.length - 1];
                                        file.seekg(now.son[0]);
                                        //node tmp_0;
                                        file.read(reinterpret_cast<char *>(&tmp_0), sizeof(tmp_0));
                                        tmp_0.father = now.address;
                                        file.seekp(now.son[0]);
                                        file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                        tmp.value[tmp.length - 2] = tmp_.value[tmp_.length - 2];
                                        tmp_.length--;
                                        file.seekp(now.address);
                                        file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                        file.seekp(tmp.address);
                                        file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                        file.seekp(tmp_.address);
                                        file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                        break;
                                    } else {
                                        delete_num++;
                                        delete_.push_back(now.address);//!删除
                                        tmp_.value[tmp_.length - 1] = tmp.value[tmp.length - 2];
                                        for (int u = 0; u < now.length - 1; u++) {
                                            tmp_.value[u + tmp_.length] = now.value[u];
                                            tmp_.son[u + tmp_.length] = now.son[u];
                                        }
                                        tmp_.length += now.length;
                                        tmp_.son[tmp_.length - 1] = now.son[now.length - 1];
                                        file.seekp(tmp_.address);
                                        file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                        for (int u = 0; u <= now.length - 1; u++) {
                                            file.seekg(tmp_.son[u + tmp_.length - now.length]);
                                            //node tmp_0;
                                            file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                            tmp_0.father = tmp_.address;
                                            file.seekp(tmp_.son[u + tmp_.length - now.length]);
                                            file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                        }
                                        tmp.length--;
                                        file.seekp(tmp.address);
                                        file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                    }
                                    now = tmp_;
                                }
                                file.seekg(now.father);
                                file.read(reinterpret_cast<char *>(&now), sizeof(node));
                            } else break;
                        }
                        if (now.father == -1) {//如果当前的节点是根节点，特殊处理
                            if (now.length == 1) {
                                file.seekg(now.son[0]);
                                file.read(reinterpret_cast<char *>(&now), sizeof(node));
                                if (now.is_leave) {
                                    file.close();
                                    return;
                                }
                                delete_num++;
                                delete_.push_back(now.address);//!删除
                                now.address = 0;
                                now.father = -1;
                                file.seekp(0);
                                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                for (int i = 0; i < now.length; i++) {
                                    //node tmp_0;
                                    file.seekg(now.son[i]);
                                    file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                    tmp_0.father = 0;
                                    file.seekp(now.son[i]);
                                    file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                }
                            }
                        }
                        file.close();
                        return;
                    }
                }//以下均为对于右边节点的处理，和上方几乎一样
                file.seekg(now.next);
                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                delete_num++;
                delete_.push_back(tmp.address);//!删除
                for (int k = 0; k < tmp.length; k++) {
                    now.value[k + now.length] = tmp.value[k];
                }
                now.length += tmp.length;
                now.next = tmp.next;
                v_up = tmp.value[0];
                file.seekp(now.address);
                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                if (now.next != -1) {
                    file.seekg(now.next);
                    //node tmp_;
                    file.read(reinterpret_cast<char *>(&tmp_), sizeof(node));
                    tmp_.pre = now.address;
                    file.seekp(tmp_.address);
                    file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                }
                file.seekg(now.father);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
                for (int k = 0; k < now.length - 1; k++) {
                    if (!cpy(v_up.first, now.value[k].first) &&
                        !cpy(now.value[k].first, v_up.first)) {
                        for (int u = k; u < now.length - 2; u++) {
                            now.value[u] = now.value[u + 1];
                            now.son[u + 1] = now.son[u + 2];
                        }
                        break;
                    }
                }
                now.length--;
                file.seekp(now.address);
                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                while (now.father != -1) {
                    if (now.length <= (M - 2) / 2) {
                        file.seekg(now.father);
                        file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                        int k;
                        for (k = 0; k < tmp.length - 1; k++) {
                            if (cpy(now.value[0].first, tmp.value[k].first))
                                break;
                        }
                        if (k != tmp.length - 1) {
                            file.seekg(tmp.son[k + 1]);
                            //node tmp_;
                            file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                            if (tmp_.length > (M) / 2) {
                                now.length++;
                                now.value[now.length - 2] = tmp.value[k];
                                now.son[now.length - 1] = tmp_.son[0];
                                file.seekg(now.son[now.length - 1]);
                                //node tmp_0;
                                file.read(reinterpret_cast<char *>(&tmp_0), sizeof(tmp_0));
                                tmp_0.father = now.address;
                                file.seekp(now.son[now.length - 1]);
                                file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                tmp.value[k] = tmp_.value[0];
                                for (int u = 0; u < tmp_.length - 2; u++) {
                                    tmp_.value[u] = tmp_.value[u + 1];
                                    tmp_.son[u] = tmp_.son[u + 1];
                                }
                                tmp_.son[tmp_.length - 2] = tmp_.son[tmp_.length - 1];
                                tmp_.length--;
                                file.seekp(now.address);
                                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                file.seekp(tmp.address);
                                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                file.seekp(tmp_.address);
                                file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                break;
                            } else {
                                delete_num++;
                                delete_.push_back(tmp_.address);//!删除
                                now.value[now.length - 1] = tmp.value[k];
                                for (int u = 0; u < tmp_.length - 1; u++) {
                                    now.value[u + now.length] = tmp_.value[u];
                                    now.son[u + now.length] = tmp_.son[u];
                                }
                                now.length += tmp_.length;
                                now.son[now.length - 1] = tmp_.son[tmp_.length - 1];
                                file.seekp(now.address);
                                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                for (int u = 0; u <= tmp_.length - 1; u++) {
                                    file.seekg(now.son[u + now.length - tmp_.length]);
                                    //node tmp_0;
                                    file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                    tmp_0.father = now.address;
                                    file.seekp(now.son[u + now.length - tmp_.length]);
                                    file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                }
                                for (int u = k; u < tmp.length - 2; u++) {
                                    tmp.value[u] = tmp.value[u + 1];
                                    tmp.son[u + 1] = tmp.son[u + 2];
                                }
                                tmp.length--;
                                file.seekp(tmp.address);
                                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                            }
                        } else {
                            file.seekg(tmp.son[k - 1]);
                            //node tmp_;
                            file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                            if (tmp_.length > (M) / 2) {
                                now.son[now.length] = now.son[now.length - 1];
                                for (int u = now.length - 1; u >= 1; u--) {
                                    now.son[u] = now.son[u - 1];
                                    now.value[u] = now.value[u - 1];
                                }
                                ++now.length;
                                now.value[0] = tmp.value[tmp.length - 2];
                                now.son[0] = tmp_.son[tmp_.length - 1];
                                file.seekg(now.son[0]);
                                //node tmp_0;
                                file.read(reinterpret_cast<char *>(&tmp_0), sizeof(tmp_0));
                                tmp_0.father = now.address;
                                file.seekp(now.son[0]);
                                file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                tmp.value[tmp.length - 2] = tmp_.value[tmp_.length - 2];
                                tmp_.length--;
                                file.seekp(now.address);
                                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                file.seekp(tmp.address);
                                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                file.seekp(tmp_.address);
                                file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                break;
                            } else {
                                delete_num++;
                                delete_.push_back(now.address);//!删除
                                tmp_.value[tmp_.length - 1] = tmp.value[tmp.length - 2];
                                for (int u = 0; u < now.length - 1; u++) {
                                    tmp_.value[u + tmp_.length] = now.value[u];
                                    tmp_.son[u + tmp_.length] = now.son[u];
                                }
                                tmp_.length += now.length;
                                tmp_.son[tmp_.length - 1] = now.son[now.length - 1];
                                file.seekp(tmp_.address);
                                file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                for (int u = 0; u <= now.length - 1; u++) {
                                    file.seekg(tmp_.son[u + tmp_.length - now.length]);
                                    //node tmp_0;
                                    file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                    tmp_0.father = tmp_.address;
                                    file.seekp(tmp_.son[u + tmp_.length - now.length]);
                                    file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                }
                                tmp.length--;
                                file.seekp(tmp.address);
                                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                            }
                            now = tmp_;
                        }
                        file.seekg(now.father);
                        file.read(reinterpret_cast<char *>(&now), sizeof(node));
                    } else break;
                }
                if (now.father == -1) {
                    if (now.length == 1) {
                        file.seekg(now.son[0]);
                        file.read(reinterpret_cast<char *>(&now), sizeof(node));
                        if (now.is_leave) {
                            file.close();
                            return;
                        }
                        delete_num++;
                        delete_.push_back(now.address);//!删除
                        now.address = 0;
                        now.father = -1;
                        file.seekp(0);
                        file.write(reinterpret_cast<char *>(&now), sizeof(node));
                        for (int i = 0; i < now.length; i++) {
                            //node tmp_0;
                            file.seekg(now.son[i]);
                            file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                            tmp_0.father = 0;
                            file.seekp(now.son[i]);
                            file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                        }
                    }
                }
                file.close();
                return;
            }
            if (!now.length) {//如果当前节点没有长度，那么肯定是根节点下方只有一个儿子（叶节点）的情况(左右都没有节点，因此不会进入上述情况），此时把根节点长度设置为0
                delete_num++;
                delete_.push_back(now.address);//!删除
                root.length--;
                file.seekp(0);
                file.write(reinterpret_cast<char *>(&root), sizeof(node));
            }
        }

    public:
        explicit bpt(std::string file_name_, std::string file_delete_name_) {
            file_name = file_name_;
            file_delete_name = file_delete_name_;
            file.open(file_name);
            file_delete.open(file_delete_name);
            if (!file) {
                file.open(file_name, std::fstream::out);
                file_delete.open(file_delete_name, std::fstream::out);
                //node tmp;
                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                file.close();
                int x = 0;
                file_delete.write(reinterpret_cast<char *>(&x), sizeof(int));
                file_delete.close();
                return;
            }
            file_delete.read(reinterpret_cast<char *>(&delete_num), sizeof(int));
            for (int i = 1; i <= delete_num; i++) {
                long long x;
                file_delete.read(reinterpret_cast<char *>(&x), sizeof(long long));
                delete_.push_back(x);
            }
            file_delete.close();
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            file.close();
        }

        ~bpt() {
            file_delete.open(file_delete_name);
            file_delete.write(reinterpret_cast<char *>(&delete_num), sizeof(int));
            for (int i = 0; i < delete_.size(); i++) {
                file_delete.write(reinterpret_cast<char *>(&delete_[i]), sizeof(long long));
            }
            file_delete.close();
        }

        void insert(const value_type &value) {
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            if (!root.length) {//无根的时候添加一个根
                //node tmp;
                tmp = ept;//!
                if (!delete_num) {
                    file.seekg(0, std::ios::end);
                } else {
                    long long x = delete_.back();
                    delete_.pop_back();
                    delete_num--;
                    file.seekg(x);
                }
                tmp.address = file.tellg();
                root.son[0] = tmp.address;
                tmp.father = 0;
                tmp.is_leave = 1;
                root.length++;
                tmp.length++;
                tmp.value[0] = value;
                file.seekp(tmp.address);
                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                file.seekp(0);
                file.write(reinterpret_cast<char *>(&root), sizeof(node));
                file.close();
                return;
            }
            now = root;//!
            while (1) {
                int i;
                for (i = 0; i < now.length - 1; i++) //比较来寻找下一层
                    if (cpy(value.first, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));//!
                if (tmp.is_leave) {//判断是不是叶节点
                    //insert_leave(value, tmp, now);//now为tmp的父亲
                    int i;
                    for (i = 0; i < tmp.length; i++) {//添加节点
                        if (!cpy(value.first, tmp.value[i].first) && !cpy(tmp.value[i].first, value.first)) {
                            file.close();
                            throw int();
                        }
                        if (cpy(value.first, tmp.value[i].first)) {
                            for (int j = tmp.length - 1; j >= i; j--)//往后移动
                                tmp.value[j + 1] = tmp.value[j];
                            tmp.value[i] = value;
                            break;
                        }
                    }
                    if (i == tmp.length)
                        tmp.value[tmp.length] = value;
                    tmp.length++;
                    file.seekp(tmp.address);
                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                    if (tmp.length == M) {//如果叶节点个数过多则需要分裂
                        split(tmp, now);
                    }
                    break;
                } else
                    now = tmp;//直到找到叶节点
            }
            file.close();
        }

        T find(const Key &key) {//按照key进行寻找
            if (hs.count(key)) {
                return hs.find(key)->second;
            }
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            if (!root.length) throw int();
            now = root;//!
            while (!now.is_leave) {
                int i;
                for (i = 0; i < now.length - 1; i++)
                    if (cpy(key, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            for (int i = 0; i < now.length; i++)
                if (!cpy(key, now.value[i].first) && !cpy(now.value[i].first, key)) {
                    if (hs.Size() >= 3ll*1024*1024) {
                        hs.erase(hs.begin());
                    }
                    hs.insert(now.value[i]);
                    file.close();
                    return now.value[i].second;
                }
            file.close();
            throw int();
        }

        void erase(const Key &key) {
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            if (!root.length) {
                file.close();
                throw int();
            }
            now = root;//!
            while (!now.is_leave) {//先寻找到叶节点的位置
                int i;
                for (i = 0; i < now.length - 1; i++)
                    if (cpy(key, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            for (int i = 0; i < now.length; i++)
                if (!cpy(key, now.value[i].first) && !cpy(now.value[i].first, key)) {
                    if (hs.count(now.value[i].first)) {
                        hs.erase(hs.find(now.value[i].first));
                    }
                    value_type v_up = now.value[0];
                    for (int j = i; j < now.length - 1; j++)
                        now.value[j] = now.value[j + 1];
                    now.length--;
                    file.seekp(now.address);
                    file.write(reinterpret_cast<char *>(&now), sizeof(node));
                    if (!i) {//如果删的是第一个数字，需要将父亲（可能比父亲还高级）的索引更改
                        tmp = now;//!
                        bool ok = 0;//是否找到需要更改的索引
                        while (tmp.father != -1) {
                            file.seekg(tmp.father);
                            file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                            for (int j = 0; j < tmp.length - 1; j++) {
                                if (!cpy(v_up.first, tmp.value[j].first) && !cpy(tmp.value[j].first, v_up.first)) {
                                    tmp.value[j] = now.value[0];
                                    v_up = now.value[0];//!
                                    file.seekp(tmp.address);
                                    file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                    ok = 1;
                                    break;
                                }
                            }
                            if (ok) break;
                        }
                    }
                    if (now.length <= (M - 2) / 2) {
                        merge(now, v_up);
                    }
                    file.close();
                    return;
                }
            file.close();
            throw int();
        }

        void modify(const Key &key, const T &v) {
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            now = root;//!
            while (!now.is_leave) {
                int i;
                for (i = 0; i < now.length - 1; i++)
                    if (cpy(key, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            for (int i = 0; i < now.length; i++)
                if (!cpy(key, now.value[i].first) && !cpy(now.value[i].first, key)) {
                    if (hs.count(now.value[i].first)) {
                        hs.erase(hs.find(now.value[i].first));
                    }
                    now.value[i].second = v;
                    hs.insert(now.value[i]);
                    file.seekp(now.address);
                    file.write(reinterpret_cast<char *>(&now), sizeof(node));
                    file.close();
                    return;
                }
            file.close();
            throw int();
        }

        bool count(const Key &key) {
            try {
                find(key);
                return true;
            } catch (...) { return false; }
        }

        bool empty() {
            return root.length == 0;
        }

        T &operator[](const Key &key) {
            try {
                T ans = find(key);
                return ans;
            } catch (...) { throw int(); }
        }

        void clean() {
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            root.length = 0;
            file.write(reinterpret_cast<char *>(&root), sizeof(node));
            file.close();
        }

        Key lower_bound(const Key &key) {
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            now = root;//!
            while (!now.is_leave) {
                int i;
                for (i = 0; i < now.length - 1; i++)
                    if (cpy(key, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            for (int i = 0; i < now.length; i++)
                if (!cpy(now.value[i].first, key)) {
                    file.close();
                    return now.value[i].first;
                }
            file.close();
            return key;
        }

        Key upper_bound(const Key &key) {
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            now = root;//!
            while (!now.is_leave) {
                int i;
                for (i = 0; i < now.length - 1; i++)
                    if (cpy(key, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            for (int i = 0; i < now.length; i++)
                if (cpy(key, now.value[i].first)) {
                    file.close();
                    return now.value[i].first;
                }
            if (now.next == -1) {
                file.close();
                return key;
            } else {
                file.seekg(now.next);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
                file.close();
                return now.value[0].first;
            }
        }

        sjtu::vector<value_type> traverse_val(const Key &l, const Key &r) {
            sjtu::vector<value_type> vec_tmp;
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            now = root;//!
            while (!now.is_leave) {
                int i;
                for (i = 0; i < now.length - 1; i++)
                    if (cpy(l, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            int i;
            for (i = 0; i < now.length; i++)
                if (!cpy(now.value[i].first, l)) {
                    //vec_tmp.push_back(now.value[i].second);
                    break;
                }
            while (1) {
                for (int j = i; j < now.length; j++) {
                    if (!cpy(r, now.value[j].first))
                        vec_tmp.push_back(now.value[j]);
                    else {
                        file.close();
                        return vec_tmp;
                    }
                }
                if (now.next == -1) break;
                file.seekg(now.next);
                file.read(reinterpret_cast<char *>(&now), sizeof(now));
                i = 0;
            }
            file.close();
            return vec_tmp;
        }

        sjtu::vector<T> traverse(const Key &l, const Key &r) {
            sjtu::vector<T> vec_tmp;
            file.open(file_name);
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            now = root;//!
            while (!now.is_leave) {
                int i;
                for (i = 0; i < now.length - 1; i++)
                    if (cpy(l, now.value[i].first))
                        break;
                file.seekg(now.son[i]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            int i;
            for (i = 0; i < now.length; i++)
                if (!cpy(now.value[i].first, l)) {
                    //vec_tmp.push_back(now.value[i].second);
                    break;
                }
            while (1) {
                for (int j = i; j < now.length; j++) {
                    if (!cpy(r, now.value[j].first))
                        vec_tmp.push_back(now.value[j].second);
                    else {
                        file.close();
                        return vec_tmp;
                    }
                }
                if (now.next == -1) break;
                file.seekg(now.next);
                file.read(reinterpret_cast<char *>(&now), sizeof(now));
                i = 0;
            }
            file.close();
            return vec_tmp;
        }
        
    };
}

#endif //MAIN_MAP_CPP_BPT_H
