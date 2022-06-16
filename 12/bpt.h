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

template<>
struct std::hash<sjtu::pair<size_t, int>> {
    size_t operator()(const sjtu::pair<size_t, int> &key) const {
        return hash<size_t>()(key.first) + hash<int>()(key.second);
    }
};

template<>
struct std::hash<sjtu::pair<size_t, size_t>> {
    size_t operator()(const sjtu::pair<size_t, size_t> &key) const {
        return hash<size_t>()(key.first) + hash<size_t>()(key.second);
    }
};

template<>
struct std::hash<sjtu::pair<sjtu::pair<size_t, int>, int>> {
    size_t operator()(const sjtu::pair<sjtu::pair<size_t, int>, int> &key) const {
        return hash<size_t>()(key.first.first) + hash<int>()(key.first.second) + hash<int>()(key.second);
    }
};
namespace sjtu {
    template<class Key, class T, int M = 100, int N = 300, class Hash=std::hash<Key>, class Compare= std::less<Key> >
    class bpt {
    private:
        typedef pair<Key, T> value_type;
        struct node {
            bool is_leave = 0;
            int length = 0;
            long long address = 8;
            long long father = -1;
            Key value[M];
            long long son[M];
            long long leave_address = -1;
            long long next_delete = -1;
        };
        struct node_leaves {
            int length = 0;
            value_type value[N];
            long long pre = -1;
            long long next = -1;
            long long address = -1;
            long long key_address = -1;
            long long next_delete = -1;
        };
        node root;
        node node_back;
        node tmp_;
        node now;
        node tmp;
        node tmp_0;
        node new_tmp;
        node ept;
        node_leaves tmp_l_;
        node_leaves now_l;
        node_leaves tmp_l;
        node_leaves tmp_0_l;
        node_leaves new_tmp_l;
        node_leaves ept_l;
        sjtu::linked_hashmap<Key, T, Hash> hs;
        //sjtu::map<Key, T> hs;
        static const int m = 74669;
        std::fstream file;
        //std::fstream file_delete;
        std::fstream file_leaves;
        //std::fstream file_delete_leaves;
        std::string file_name;
        //std::string file_delete_name;
        std::string file_leaves_name;
        //std::string file_delete_leaves_name;
        Compare cpy;
        sjtu::vector<long long> delete_;
        int delete_num = 0;
        sjtu::vector<long long> delete_leaves;
        int delete_leaves_num = 0;

        void split(node &tmp, node &now) {
            value_type v_up;
            //tmp.length = N >> 1;
            tmp_l.length = N >> 1;
            //!
            //file.seekp(tmp.address);
            //file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
            //!
            /*if (!delete_num) {
                file.seekp(0, std::fstream::end);
            } else {
                long long x = delete_.back();
                delete_.pop_back();
                delete_num--;
                file.seekg(x);
            }*/
            //!
            long long x;
            file.seekg(0);
            file.read(reinterpret_cast<char *>(&x), sizeof(long long));
            if (!x) {
                file.seekg(0, std::ios::end);
            } else {
                file.seekg(x);
                file.read(reinterpret_cast<char *>(&new_tmp), sizeof(node));
                long long x_ = 0;
                if (new_tmp.next_delete != -1) x_ = new_tmp.next_delete;
                file.seekp(0);
                file.write(reinterpret_cast<char *>(&x_), sizeof(long long));
                file.seekg(x);
            }
            //!
            new_tmp = ept;//置空
            new_tmp.address = file.tellp();//新取一个地址 可记录
            new_tmp.father = now.address;
            //!
            /*if (!delete_leaves_num) {
                file_leaves.seekp(0, std::fstream::end);
            } else {
                long long x = delete_leaves.back();
                delete_leaves.pop_back();
                delete_leaves_num--;
                file_leaves.seekg(x);
            }*/
            //!
            file_leaves.seekg(0);
            file_leaves.read(reinterpret_cast<char *>(&x), sizeof(long long));
            if (!x) {
                file_leaves.seekg(0, std::ios::end);
            } else {
                file_leaves.seekg(x);
                file_leaves.read(reinterpret_cast<char *>(&new_tmp_l), sizeof(node_leaves));
                long long x_ = 0;
                if (new_tmp_l.next_delete != -1) x_ = new_tmp_l.next_delete;
                file_leaves.seekp(0);
                file_leaves.write(reinterpret_cast<char *>(&x_), sizeof(long long));
                file_leaves.seekg(x);
            }
            //!
            new_tmp_l = ept_l;//置空
            new_tmp.leave_address = file_leaves.tellp();
            new_tmp_l.address = new_tmp.leave_address;
            new_tmp_l.key_address = new_tmp.address;
            //!
            if (tmp_l.next != -1) {//先将叶子节点的链表连起来
                file_leaves.seekg(tmp_l.next);
                file_leaves.read(reinterpret_cast<char *>(&tmp_0_l), sizeof(node_leaves));
                tmp_0_l.pre = new_tmp.leave_address;
                new_tmp_l.next = tmp_0_l.address;
                file_leaves.seekp(tmp_0_l.address);
                file_leaves.write(reinterpret_cast<char *>(&tmp_0_l), sizeof(node_leaves));
            }
            tmp_l.next = new_tmp_l.address;
            new_tmp_l.pre = tmp_l.address;
            file_leaves.seekp(tmp_l.address);
            file_leaves.write(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
            //!
            for (int i = 0; i < N - tmp_l.length; i++)//将原来tmp中的值拿一半到new_tmp中
                //new_tmp.value[i] = tmp.value[i + tmp.length],
                new_tmp_l.value[i] = tmp_l.value[i + tmp_l.length];
            new_tmp_l.length = N - tmp_l.length;
            //!
            //new_tmp.length = N - tmp.length;
            new_tmp.is_leave = 1;
            file.seekp(new_tmp.address);
            file.write(reinterpret_cast<char *>(&new_tmp), sizeof(node));
            //!
            file_leaves.seekp(new_tmp_l.address);
            file_leaves.write(reinterpret_cast<char *>(&new_tmp_l), sizeof(node_leaves));
            //!
            v_up = new_tmp_l.value[0];//记录用来改变非叶节点的索引
            bool root_ = 1;//判断需不需要对根进行操作，用于下方第二个while，若根的儿子有分裂，则需要进行操作，初始设为1为了处理根的儿子是叶子节点的情况
            node_back = new_tmp;
            while (node_back.father) {//如果新添加的节点new_tmp有父亲，则对他的父亲的索引进行修改。
                root_ = 0;
                file.seekg(node_back.father);
                file.read(reinterpret_cast<char *>(&node_back), sizeof(node));
                int l = 0, r = node_back.length - 2;
                while (l <= r) {
                    int mid = (l + r) >> 1;
                    if (cpy(new_tmp_l.value[0].first, node_back.value[mid])) r = mid - 1;
                    else l = mid + 1;
                }
                for (int j = node_back.length - 1; j >= l; j--) {
                    node_back.value[j + 1] = node_back.value[j];
                    node_back.son[j + 1] = node_back.son[j];
                }
                node_back.value[l] = v_up.first;
                node_back.son[l + 1] = new_tmp.address;
                if (l + 1 == node_back.length) {
                    node_back.value[node_back.length - 1] = v_up.first;
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
                    v_up.first = node_back.value[node_back.length - 1];
                    new_tmp.length = M - node_back.length;
                    new_tmp.son[new_tmp.length - 1] = node_back.son[M - 1];
                    /*if (!delete_num) {
                        file.seekp(0, std::fstream::end);
                    } else {
                        long long x = delete_.back();
                        delete_.pop_back();
                        delete_num--;
                        file.seekg(x);
                    }*/
                    //!
                    long long x;
                    file.seekg(0);
                    file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                    if (!x) {
                        file.seekg(0, std::ios::end);
                    } else {
                        file.seekg(x);
                        file.read(reinterpret_cast<char *>(&new_tmp), sizeof(node));
                        long long x_ = 0;
                        if (new_tmp.next_delete != -1) x_ = new_tmp.next_delete;
                        file.seekp(0);
                        file.write(reinterpret_cast<char *>(&x_), sizeof(long long));
                        file.seekg(x);
                        new_tmp = ept;
                    }
                    //!
                    new_tmp.address = file.tellg();//新取一个地址 可记录
                    new_tmp.father = node_back.father;//!!
                    new_tmp.is_leave = 0;
                    file.write(reinterpret_cast<char *>(&new_tmp), sizeof(node));
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
                int l = 0, r = node_back.length - 2;
                while (l <= r) {
                    int mid = (l + r) >> 1;
                    if (cpy(new_tmp_l.value[0].first, node_back.value[mid])) r = mid - 1;//!!!!!!
                    else l = mid + 1;
                }
                for (int j = node_back.length - 1; j >= l; j--) {
                    node_back.value[j + 1] = node_back.value[j];//第一个无用移动
                    node_back.son[j + 1] = node_back.son[j];//最后一个无效移动
                }
                node_back.value[l] = v_up.first;
                node_back.son[l + 1] = new_tmp.address;
                if (l + 1 == node_back.length) {
                    node_back.value[node_back.length - 1] = v_up.first;
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
                    v_up.first = node_back.value[node_back.length - 1];
                    new_tmp.length = M - node_back.length;
                    new_tmp.son[new_tmp.length - 1] = node_back.son[M - 1];
                    /*if (!delete_num) {
                        file.seekp(0, std::fstream::end);
                    } else {
                        long long x = delete_.back();
                        delete_.pop_back();
                        delete_num--;
                        file.seekg(x);
                    }*/
                    //!
                    long long x;
                    file.seekg(0);
                    file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                    if (!x) {
                        file.seekg(0, std::ios::end);
                    } else {
                        file.seekg(x);
                        file.read(reinterpret_cast<char *>(&new_tmp), sizeof(node));
                        long long x_ = 0;
                        if (new_tmp.next_delete != -1) x_ = new_tmp.next_delete;
                        file.seekp(0);
                        file.write(reinterpret_cast<char *>(&x_), sizeof(long long));
                        file.seekg(x);
                        new_tmp = ept;
                    }
                    //!
                    new_tmp.address = file.tellg();//新取一个地址 可记录
                    new_tmp.father = 8;
                    new_tmp.is_leave = 0;
                    file.write(reinterpret_cast<char *>(&new_tmp), sizeof(node));
                    for (int i = 0; i < new_tmp.length; i++) {
                        file.seekg(new_tmp.son[i]);
                        file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                        tmp_.father = new_tmp.address;
                        file.seekp(new_tmp.son[i]);
                        file.write(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                    }
                    /*if (!delete_num) {
                        file.seekp(0, std::fstream::end);
                    } else {
                        long long x = delete_.back();
                        delete_.pop_back();
                        delete_num--;
                        file.seekg(x);
                    }*/
                    //!
                    file.seekg(0);
                    file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                    if (!x) {
                        file.seekg(0, std::ios::end);
                    } else {
                        file.seekg(x);
                        file.read(reinterpret_cast<char *>(&node_back), sizeof(node));
                        long long x_ = 0;
                        if (node_back.next_delete != -1) x_ = node_back.next_delete;
                        file.seekp(0);
                        file.write(reinterpret_cast<char *>(&x_), sizeof(long long));
                        file.seekg(x);
                        node_back = ept;
                    }
                    //!
                    node_back.address = file.tellg();//新取一个地址 可记录
                    node_back.father = 8;
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
                    root.value[0] = v_up.first;
                    file.seekp(8, std::fstream::beg);
                    file.write(reinterpret_cast<char *>(&root), sizeof(node));
                }
            }
        }

        void merge(node &now, value_type &v_up) {
            if (now_l.pre != -1) {//如果合并的节点左边有节点的话
                //!
                file_leaves.seekg(now_l.pre);
                file_leaves.read(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                //file.seekg(tmp_l.key_address);
                //file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                if (tmp_l.length > (N) / 2) {//如果左边的节点可以移动一个节点过来
                    now_l.length++;
                    //now.length++;
                    tmp_l.length--;
                    //tmp.length--;
                    for (int k = now_l.length - 1; k >= 0; k--)
                        //now.value[k + 1] = now.value[k],
                        now_l.value[k + 1] = now_l.value[k];
                    //now.value[0] = tmp.value[tmp.length];
                    now_l.value[0] = tmp_l.value[tmp_l.length];
                    //file.seekp(tmp.address);
                    //file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                    //file.seekp(now.address);
                    //file.write(reinterpret_cast<char *>(&now), sizeof(node));
                    file_leaves.seekp(tmp_l.address);
                    file_leaves.write(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                    file_leaves.seekp(now_l.address);
                    file_leaves.write(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
                    //!
                    file.seekg(now.father);
                    file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                    bool ok = 0;
                    while (1) {//修改祖先上索引的值
                        int l = 0, r = tmp.length - 2;
                        int k;
                        bool find_it = 0;
                        while (l <= r) {
                            int mid = (l + r) >> 1;
                            if (!cpy(tmp.value[mid], v_up.first) &&
                                !cpy(v_up.first, tmp.value[mid])) {
                                find_it = 1;
                                k = mid;
                                break;
                            }
                            if (cpy(v_up.first, tmp.value[mid])) r = mid - 1;
                            else l = mid + 1;
                        }
                        if (find_it) {
                            tmp.value[k] = now_l.value[0].first;
                            file.seekp(tmp.address);
                            file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                            ok = 1;
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
            if (now_l.next != -1) {//同理判断右边的节点
                file_leaves.seekg(now_l.next);
                file_leaves.read(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                //file.seekg(tmp_l.key_address);
                //file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                if (tmp_l.length > (N) / 2) {
                    //now.length++;
                    now_l.length++;
                    //tmp.length--;
                    tmp_l.length--;
                    //now.value[now.length - 1] = tmp.value[0];
                    now_l.value[now_l.length - 1] = tmp_l.value[0];
                    value_type up = tmp_l.value[0];
                    for (int k = 0; k <= tmp_l.length - 1; k++)
                        //tmp.value[k] = tmp.value[k + 1],
                        tmp_l.value[k] = tmp_l.value[k + 1];
                    //file.seekp(tmp.address);
                    //file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                    //file.seekp(now.address);
                    //file.write(reinterpret_cast<char *>(&now), sizeof(node));
                    file_leaves.seekp(tmp_l.address);
                    file_leaves.write(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                    file_leaves.seekp(now_l.address);
                    file_leaves.write(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
                    //!
                    file.seekg(now.father);
                    file.read(reinterpret_cast<char *>(&now), sizeof(node));
                    bool ok = 0;
                    while (1) {
                        int l = 0, r = now.length - 2;
                        int k;
                        bool find_it = 0;
                        while (l <= r) {
                            int mid = (l + r) >> 1;
                            if (!cpy(now.value[mid], up.first) &&
                                !cpy(up.first, now.value[mid])) {
                                find_it = 1;
                                k = mid;
                                break;
                            }
                            if (cpy(up.first, now.value[mid])) r = mid - 1;
                            else l = mid + 1;
                        }
                        if (find_it) {
                            now.value[k] = tmp_l.value[0].first;
                            file.seekp(now.address);
                            file.write(reinterpret_cast<char *>(&now), sizeof(node));
                            ok = 1;
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
            if (now_l.pre != -1 || now_l.next != -1) {//以下判断的是需要合并的情况
                if (now_l.pre != -1) {//如果左边有节点，由上述判断知道肯定只能合并
                    //!
                    file_leaves.seekg(now_l.pre);
                    file_leaves.read(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                    file.seekg(tmp_l.key_address);
                    file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                    if (now.father == tmp.father) {//如果两个节点父亲是同一个节点，否则就考虑右边的节点，此时右边的节点肯定是同一个父亲
                        /*delete_num++;
                        delete_.push_back(now.address);
                        //!
                        delete_leaves_num++;
                        delete_leaves.push_back(now_l.address);
                        //!*/
                        //!
                        long long x;
                        file.seekg(0);
                        file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                        file.seekp(0);
                        file.write(reinterpret_cast<char *>(&now.address), sizeof(long long));
                        if (x) now.next_delete = x; else now.next_delete = -1;
                        file.seekp(now.address);
                        file.write(reinterpret_cast<char *>(&now), sizeof(node));
                        file_leaves.seekg(0);
                        file_leaves.read(reinterpret_cast<char *>(&x), sizeof(long long));
                        file_leaves.seekp(0);
                        file_leaves.write(reinterpret_cast<char *>(&now_l.address), sizeof(long long));
                        if (x) now_l.next_delete = x; else now_l.next_delete = -1;
                        file_leaves.seekp(now_l.address);
                        file_leaves.write(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
                        //!
                        for (int k = 0; k < now_l.length; k++) {
                            //tmp.value[k + tmp.length] = now.value[k];
                            tmp_l.value[k + tmp_l.length] = now_l.value[k];
                        }
                        //tmp.length += now.length;
                        tmp_l.length += now_l.length;
                        tmp_l.next = now_l.next;
                        //!
                        //file.seekp(tmp.address);
                        //file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                        file_leaves.seekp(tmp_l.address);
                        file_leaves.write(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                        if (tmp_l.next != -1) {
                            file_leaves.seekg(tmp_l.next);
                            file_leaves.read(reinterpret_cast<char *>(&tmp_l_), sizeof(node_leaves));
                            tmp_l_.pre = tmp_l.address;
                            file_leaves.seekp(tmp_l_.address);
                            file_leaves.write(reinterpret_cast<char *>(&tmp_l_), sizeof(node_leaves));
                        }
                        file.seekg(now.father);
                        file.read(reinterpret_cast<char *>(&now), sizeof(node));
                        int l = 0, r = now.length - 2;
                        int k;
                        bool find_it = 0;
                        while (l <= r) {
                            int mid = (l + r) >> 1;
                            if (!cpy(now.value[mid], v_up.first) &&
                                !cpy(v_up.first, now.value[mid])) {
                                find_it = 1;
                                k = mid;
                                break;
                            }
                            if (cpy(v_up.first, now.value[mid])) r = mid - 1;
                            else l = mid + 1;
                        }
                        if (find_it) {
                            for (int u = k; u < now.length - 2; u++) {
                                now.value[u] = now.value[u + 1];
                                now.son[u + 1] = now.son[u + 2];
                            }
                        }
                        now.length--;
                        file.seekp(now.address);
                        file.write(reinterpret_cast<char *>(&now), sizeof(node));
                        while (now.father != -1) {//寻找祖先进行合并
                            if (now.length <= (M - 2) / 2) {//如果这个点需要合并，则执行和上面一样的操作，否则直接break
                                file.seekg(now.father);
                                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                                l = 0, r = tmp.length - 2;
                                while (l <= r) {
                                    int mid = (l + r) >> 1;
                                    if (cpy(now.value[0], tmp.value[mid])) r = mid - 1;
                                    else l = mid + 1;
                                }
                                if (l != tmp.length - 1) {//如果不是最后一个
                                    file.seekg(tmp.son[l + 1]);
                                    //node tmp_;
                                    file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                                    if (tmp_.length > (M) / 2) {//如果可以挪一个数据过来
                                        now.length++;
                                        now.value[now.length - 2] = tmp.value[l];
                                        now.son[now.length - 1] = tmp_.son[0];
                                        file.seekg(now.son[now.length - 1]);
                                        //node tmp_0;
                                        file.read(reinterpret_cast<char *>(&tmp_0), sizeof(tmp_0));
                                        tmp_0.father = now.address;
                                        file.seekp(now.son[now.length - 1]);
                                        file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                        tmp.value[l] = tmp_.value[0];
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
                                        /*delete_num++;
                                        delete_.push_back(tmp_.address);//!删除*/
                                        //!
                                        long long x;
                                        file.seekg(0);
                                        file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                                        file.seekp(0);
                                        file.write(reinterpret_cast<char *>(&tmp_.address), sizeof(long long));
                                        if (x) tmp_.next_delete = x; else tmp_.next_delete = -1;
                                        file.seekp(tmp_.address);
                                        file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                        //!
                                        now.value[now.length - 1] = tmp.value[l];
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
                                        for (int u = l; u < tmp.length - 2; u++) {
                                            tmp.value[u] = tmp.value[u + 1];
                                            tmp.son[u + 1] = tmp.son[u + 2];
                                        }
                                        tmp.length--;
                                        file.seekp(tmp.address);
                                        file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                                    }
                                } else {
                                    file.seekg(tmp.son[l - 1]);
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
                                        /*delete_num++;
                                        delete_.push_back(now.address);//!删除*/
                                        //!
                                        long long x;
                                        file.seekg(0);
                                        file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                                        file.seekp(0);
                                        file.write(reinterpret_cast<char *>(&now.address), sizeof(long long));
                                        if (x) now.next_delete = x; else now.next_delete = -1;
                                        file.seekp(now.address);
                                        file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                        //!
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
                                /*delete_num++;
                                delete_.push_back(now.address);//!删除*/
                                //!
                                long long x;
                                file.seekg(0);
                                file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                                file.seekp(0);
                                file.write(reinterpret_cast<char *>(&now.address), sizeof(long long));
                                if (x) now.next_delete = x; else now.next_delete = -1;
                                //!
                                now.address = 8;
                                now.father = -1;
                                file.seekp(8);
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
                //!
                file_leaves.seekg(now_l.next);
                file_leaves.read(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                file.seekg(tmp_l.key_address);
                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                /*delete_num++;
                delete_.push_back(tmp.address);
                delete_leaves_num++;
                delete_leaves.push_back(tmp_l.address);*/
                //!
                long long x;
                file.seekg(0);
                file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                file.seekp(0);
                file.write(reinterpret_cast<char *>(&tmp.address), sizeof(long long));
                if (x) tmp.next_delete = x; else tmp.next_delete = -1;
                file.seekp(tmp.address);
                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                file_leaves.seekg(0);
                file_leaves.read(reinterpret_cast<char *>(&x), sizeof(long long));
                file_leaves.seekp(0);
                file_leaves.write(reinterpret_cast<char *>(&tmp_l.address), sizeof(long long));
                if (x) tmp_l.next_delete = x; else tmp_l.next_delete = -1;
                file_leaves.seekp(tmp_l.address);
                file_leaves.write(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                //!
                for (int k = 0; k < tmp_l.length; k++) {
                    //now.value[k + now.length] = tmp.value[k];
                    now_l.value[k + now_l.length] = tmp_l.value[k];
                }
                //now.length += tmp.length;
                now_l.length += tmp_l.length;
                now_l.next = tmp_l.next;
                v_up = tmp_l.value[0];
                //file.seekp(now.address);
                //file.write(reinterpret_cast<char *>(&now), sizeof(node));
                file_leaves.seekp(now_l.address);
                file_leaves.write(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
                if (now_l.next != -1) {
                    file_leaves.seekg(now_l.next);
                    file_leaves.read(reinterpret_cast<char *>(&tmp_l_), sizeof(node_leaves));
                    tmp_l_.pre = now_l.address;
                    file_leaves.seekp(tmp_l_.address);
                    file_leaves.write(reinterpret_cast<char *>(&tmp_l_), sizeof(node_leaves));
                }
                //!
                file.seekg(now.father);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
                int l = 0, r = now.length - 2;
                int k;
                bool find_it = 0;
                while (l <= r) {
                    int mid = (l + r) >> 1;
                    if (!cpy(now.value[mid], v_up.first) &&
                        !cpy(v_up.first, now.value[mid])) {
                        find_it = 1;
                        k = mid;
                        break;
                    }
                    if (cpy(v_up.first, now.value[mid])) r = mid - 1;
                    else l = mid + 1;
                }
                if (find_it) {
                    for (int u = k; u < now.length - 2; u++) {
                        now.value[u] = now.value[u + 1];
                        now.son[u + 1] = now.son[u + 2];
                    }
                }
                now.length--;
                file.seekp(now.address);
                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                while (now.father != -1) {
                    if (now.length <= (M - 2) / 2) {
                        file.seekg(now.father);
                        file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                        l = 0, r = tmp.length - 2;
                        while (l <= r) {
                            int mid = (l + r) >> 1;
                            if (cpy(now.value[0], tmp.value[mid])) r = mid - 1;
                            else l = mid + 1;
                        }
                        if (l != tmp.length - 1) {
                            file.seekg(tmp.son[l + 1]);
                            file.read(reinterpret_cast<char *>(&tmp_), sizeof(tmp_));
                            if (tmp_.length > (M) / 2) {
                                now.length++;
                                now.value[now.length - 2] = tmp.value[l];
                                now.son[now.length - 1] = tmp_.son[0];
                                file.seekg(now.son[now.length - 1]);
                                file.read(reinterpret_cast<char *>(&tmp_0), sizeof(tmp_0));
                                tmp_0.father = now.address;
                                file.seekp(now.son[now.length - 1]);
                                file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                tmp.value[l] = tmp_.value[0];
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
                                /*delete_num++;
                                delete_.push_back(tmp_.address);*/
                                //!
                                long long x;
                                file.seekg(0);
                                file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                                file.seekp(0);
                                file.write(reinterpret_cast<char *>(&tmp_.address), sizeof(long long));
                                if (x) tmp_.next_delete = x; else tmp_.next_delete = -1;
                                file.seekp(tmp_.address);
                                file.write(reinterpret_cast<char *>(&tmp_), sizeof(node));
                                //!
                                now.value[now.length - 1] = tmp.value[l];
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
                                    file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                    tmp_0.father = now.address;
                                    file.seekp(now.son[u + now.length - tmp_.length]);
                                    file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                                }
                                for (int u = l; u < tmp.length - 2; u++) {
                                    tmp.value[u] = tmp.value[u + 1];
                                    tmp.son[u + 1] = tmp.son[u + 2];
                                }
                                tmp.length--;
                                file.seekp(tmp.address);
                                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                            }
                        } else {
                            file.seekg(tmp.son[l - 1]);
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
                                /*delete_num++;
                                delete_.push_back(now.address);*/
                                //!
                                long long x;
                                file.seekg(0);
                                file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                                file.seekp(0);
                                file.write(reinterpret_cast<char *>(&now.address), sizeof(long long));
                                if (x) now.next_delete = x; else now.next_delete = -1;
                                file.seekp(now.address);
                                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                                //!
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
                        /*delete_num++;
                        delete_.push_back(now.address);*/
                        //!
                        long long x;
                        file.seekg(0);
                        file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                        file.seekp(0);
                        file.write(reinterpret_cast<char *>(&now.address), sizeof(long long));
                        if (x) now.next_delete = x; else now.next_delete = -1;
                        //!
                        now.address = 8;
                        now.father = -1;
                        file.seekp(8);
                        file.write(reinterpret_cast<char *>(&now), sizeof(node));
                        for (int i = 0; i < now.length; i++) {
                            file.seekg(now.son[i]);
                            file.read(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                            tmp_0.father = 8;
                            file.seekp(now.son[i]);
                            file.write(reinterpret_cast<char *>(&tmp_0), sizeof(node));
                        }
                    }
                }
                file.close();
                return;
            }
            if (!now_l.length) {//如果当前节点没有长度，那么肯定是根节点下方只有一个儿子（叶节点）的情况(左右都没有节点，因此不会进入上述情况），此时把根节点长度设置为0
                /*delete_num++;
                delete_.push_back(now.address);
                //!
                delete_leaves_num++;
                delete_leaves.push_back(now_l.address);
                //!*/
                //!
                long long x;
                file.seekg(0);
                file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                file.seekp(0);
                file.write(reinterpret_cast<char *>(&now.address), sizeof(long long));
                if (x) now.next_delete = x; else now.next_delete = -1;
                file.seekp(now.address);
                file.write(reinterpret_cast<char *>(&now), sizeof(node));
                file_leaves.seekg(0);
                file_leaves.read(reinterpret_cast<char *>(&x), sizeof(long long));
                file_leaves.seekp(0);
                file_leaves.write(reinterpret_cast<char *>(&now_l.address), sizeof(long long));
                if (x) now_l.next_delete = x; else now_l.next_delete = -1;
                file_leaves.seekp(now_l.address);
                file_leaves.write(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
                //!
                root.length--;
                file.seekp(8);
                file.write(reinterpret_cast<char *>(&root), sizeof(node));
            }
        }

    public:
        explicit bpt(std::string file_name_, std::string file_leaves_name_/*, std::string file_delete_name_,
                     std::string file_delete_leaves_*/) {
            file_name = file_name_;
            file_leaves_name = file_leaves_name_;
            //file_delete_name = file_delete_name_;
            //file_delete_leaves_name = file_delete_leaves_;
            file.open(file_name);
            file_leaves.open(file_leaves_name);
            //file_delete.open(file_delete_name);
            //file_delete_leaves.open(file_delete_leaves_name);
            if (!file) {
                file.open(file_name, std::fstream::out);
                //file_delete.open(file_delete_name, std::fstream::out);
                file_leaves.open(file_leaves_name, std::fstream::out);
                //file_delete_leaves.open(file_delete_leaves_name, std::fstream::out);
                //file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                //file.close();
                //int x = 0;
                //file_delete_leaves.write(reinterpret_cast<char *>(&x), sizeof(int));
                //file_delete.write(reinterpret_cast<char *>(&x), sizeof(int));
                long long x_ = 0;
                file_leaves.write(reinterpret_cast<char *>(&x_), sizeof(long long));
                file.write(reinterpret_cast<char *>(&x_), sizeof(long long));//!
                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));//!
                file.close();
                //file_delete.close();
                file_leaves.close();
                //file_delete_leaves.close();
                return;
            }
            /*file_delete.seekg(0);
            file_delete.read(reinterpret_cast<char *>(&delete_num), sizeof(int));
            for (int i = 1; i <= delete_num; i++) {
                long long x;
                file_delete.read(reinterpret_cast<char *>(&x), sizeof(long long));
                delete_.push_back(x);
            }
            file_delete_leaves.seekg(0);
            file_delete_leaves.read(reinterpret_cast<char *>(&delete_leaves_num), sizeof(int));
            for (int i = 1; i <= delete_leaves_num; i++) {
                long long x;
                file_delete_leaves.read(reinterpret_cast<char *>(&x), sizeof(long long));
                delete_leaves.push_back(x);
            }
            file_delete.close();*/
            file.seekg(8);//!
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            file.close();
            file_leaves.close();
            //file_delete_leaves.close();
        }

        ~bpt() {
            /*file_delete.open(file_delete_name);
            file_delete.seekp(0);
            file_delete.write(reinterpret_cast<char *>(&delete_num), sizeof(int));
            for (int i = 0; i < delete_.size(); i++) {
                file_delete.write(reinterpret_cast<char *>(&delete_[i]), sizeof(long long));
            }
            file_delete.close();
            //!
            file_delete_leaves.open(file_delete_leaves_name);
            file_delete_leaves.seekp(0);
            file_delete_leaves.write(reinterpret_cast<char *>(&delete_leaves_num), sizeof(int));
            for (int i = 0; i < delete_leaves.size(); i++) {
                file_delete_leaves.write(reinterpret_cast<char *>(&delete_leaves[i]), sizeof(long long));
            }
            file_delete_leaves.close();*/
            //!
        }

        void insert(const value_type &value) {
            file.open(file_name);
            file.seekg(8);//!
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            file_leaves.open(file_leaves_name);
            if (!root.length) {//无根的时候添加一个根
                tmp = ept;
                /*if (!delete_num) {
                    file.seekg(0, std::ios::end);
                } else {
                    long long x = delete_.back();
                    delete_.pop_back();
                    delete_num--;
                    file.seekg(x);
                }*/
                //!
                long long x;
                file.seekg(0);
                file.read(reinterpret_cast<char *>(&x), sizeof(long long));
                if (!x) {
                    file.seekg(0, std::ios::end);
                } else {
                    file.seekg(x);
                    file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                    long long x_ = 0;
                    if (tmp.next_delete != -1) x_ = tmp.next_delete;
                    file.seekp(0);
                    file.write(reinterpret_cast<char *>(&x_), sizeof(long long));
                    file.seekg(x);
                    tmp = ept;
                }
                //!
                tmp.address = file.tellg();
                root.son[0] = tmp.address;
                tmp.father = 8;
                tmp.is_leave = 1;
                root.length++;
                //tmp.length++;
                //!
                //tmp.value[0] = value.first;
                /*if (!delete_leaves_num) {
                    file_leaves.seekg(0, std::ios::end);
                } else {
                    long long x = delete_leaves.back();
                    delete_leaves.pop_back();
                    delete_leaves_num--;
                    file_leaves.seekg(x);
                }*/
                //!
                file_leaves.seekg(0);
                file_leaves.read(reinterpret_cast<char *>(&x), sizeof(long long));
                if (!x) {
                    file_leaves.seekg(0, std::ios::end);
                } else {
                    file_leaves.seekg(x);
                    file_leaves.read(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                    long long x_ = 0;
                    if (tmp_l.next_delete != -1) x_ = tmp_l.next_delete;
                    file_leaves.seekp(0);
                    file_leaves.write(reinterpret_cast<char *>(&x_), sizeof(long long));
                    file_leaves.seekg(x);
                }
                //!
                tmp.leave_address = file_leaves.tellg();
                tmp_l.value[0] = value;
                tmp_l.length = 1;
                tmp_l.address = tmp.leave_address;
                tmp_l.key_address = tmp.address;
                //file_leaves.seekp(0);
                //file_leaves.write(reinterpret_cast<char *>(&tmp.leave_address), sizeof(long long));
                file_leaves.seekp(8);
                file_leaves.write(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                file_leaves.close();
                //!
                file.seekp(tmp.address);
                file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                file.seekp(8);
                file.write(reinterpret_cast<char *>(&root), sizeof(node));
                file.close();
                return;
            }
            now = root;
            while (1) {
                int l = 0, r = now.length - 2;
                while (l <= r) {
                    int mid = (l + r) >> 1;
                    if (cpy(value.first, now.value[mid])) r = mid - 1;
                    else l = mid + 1;
                }
                file.seekg(now.son[l]);
                file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                if (tmp.is_leave) {//判断是不是叶节点
                    file_leaves.seekg(tmp.leave_address);
                    file_leaves.read(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                    l = 0, r = tmp_l.length - 1;
                    while (l <= r) {
                        int mid = (l + r) >> 1;
                        if (!cpy(value.first, tmp_l.value[mid].first) && !cpy(tmp_l.value[mid].first, value.first)) {
                            file.close();
                            file_leaves.close();//!
                            throw int();
                        }
                        if (cpy(value.first, tmp_l.value[mid].first)) r = mid - 1;
                        else l = mid + 1;
                    }
                    /*for (int j = tmp.length - 1; j >= l; j--)//往后移动
                        tmp.value[j + 1] = tmp.value[j];
                    tmp.value[l] = value.first;
                    if (l == tmp.length)
                        tmp.value[tmp.length] = value.first;*/
                    //!
                    for (int j = tmp_l.length - 1; j >= l; j--)//往后移动
                        tmp_l.value[j + 1] = tmp_l.value[j];
                    tmp_l.value[l] = value;
                    if (l == tmp_l.length)
                        tmp_l.value[tmp_l.length] = value;
                    //tmp.length++;
                    tmp_l.length++;
                    file_leaves.seekp(tmp.leave_address);
                    file_leaves.write(reinterpret_cast<char *>(&tmp_l), sizeof(node_leaves));
                    //!
                    //file.seekp(tmp.address);
                    //file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                    if (tmp_l.length == N) {//如果叶节点个数过多则需要分裂
                        split(tmp, now);
                    }
                    break;
                } else
                    now = tmp;//直到找到叶节点
            }
            file.close();
            file_leaves.close();
        }

        inline void insert(const Key &key, const T &value) {
            insert(value_type(key, value));
        }

        sjtu::pair<bool, T> find2(const Key &key) {//按照key进行寻找
            if (hs.count(key)) {
                return sjtu::pair<bool, T>(1, hs.find(key)->second);
            }
            file.open(file_name);
            file.seekg(8);//!
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            if (!root.length) throw int();
            now = root;
            while (!now.is_leave) {
                int l = 0, r = now.length - 2;
                while (l <= r) {
                    int mid = (l + r) >> 1;
                    if (cpy(key, now.value[mid])) r = mid - 1;
                    else l = mid + 1;
                }
                file.seekg(now.son[l]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            file_leaves.open(file_leaves_name);
            file_leaves.seekg(now.leave_address);
            file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
            for (int i = 0; i < now_l.length; i++)
                if (!cpy(key, now_l.value[i].first) && !cpy(now_l.value[i].first, key)) {
                    if (hs.Size() >= 4ll * 1024 * 1024) {
                        hs.erase(hs.begin());
                    }
                    //!

                    file_leaves.close();
                    //!
                    hs.insert(now_l.value[i]);
                    file.close();
                    return sjtu::pair<bool, T>(1, now_l.value[i].second);
                }
            file_leaves.close();
            file.close();
            return sjtu::pair<bool, T>(0, T());
        }

        T find(const Key &key) {//按照key进行寻找
            if (hs.count(key)) {
                return hs.find(key)->second;
            }
            file.open(file_name);
            file.seekg(8);//!
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            if (!root.length) throw int();
            now = root;
            while (!now.is_leave) {
                int l = 0, r = now.length - 2;
                while (l <= r) {
                    int mid = (l + r) >> 1;
                    if (cpy(key, now.value[mid])) r = mid - 1;
                    else l = mid + 1;
                }
                file.seekg(now.son[l]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            file_leaves.open(file_leaves_name);
            file_leaves.seekg(now.leave_address);
            file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
            for (int i = 0; i < now_l.length; i++)
                if (!cpy(key, now_l.value[i].first) && !cpy(now_l.value[i].first, key)) {
                    if (hs.Size() >= 4ll * 1024 * 1024) {
                        hs.erase(hs.begin());
                    }
                    //!
                    file_leaves.close();
                    //!
                    hs.insert(now_l.value[i]);
                    file.close();
                    return now_l.value[i].second;
                }
            file_leaves.close();
            file.close();
            throw int();
        }

        void erase(const Key &key) {
            file.open(file_name);
            file.seekg(8);//!
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            if (!root.length) {
                file.close();
                throw int();
            }
            now = root;
            while (!now.is_leave) {//先寻找到叶节点的位置
                int l = 0, r = now.length - 2;
                while (l <= r) {
                    int mid = (l + r) >> 1;
                    if (cpy(key, now.value[mid])) r = mid - 1;
                    else l = mid + 1;
                }
                file.seekg(now.son[l]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            file_leaves.open(file_leaves_name);
            file_leaves.seekg(now.leave_address);
            file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
            int l = 0, r = now_l.length - 1;
            int i;
            bool find_it = 0;
            while (l <= r) {
                int mid = (l + r) >> 1;
                if (!cpy(key, now_l.value[mid].first) && !cpy(now_l.value[mid].first, key)) {
                    find_it = 1;
                    i = mid;
                    break;
                }
                if (cpy(key, now_l.value[mid].first)) r = mid - 1;
                else l = mid + 1;
            }
            if (find_it) {
                if (hs.count(now_l.value[i].first)) {
                    hs.erase(hs.find(now_l.value[i].first));
                }
                //!
                value_type v_up = now_l.value[0];
                for (int j = i; j < now_l.length - 1; j++)
                    //now.value[j] = now.value[j + 1],
                    now_l.value[j] = now_l.value[j + 1];
                //now.length--;
                now_l.length--;
                //file.seekp(now.address);
                //file.write(reinterpret_cast<char *>(&now), sizeof(node));
                //!
                file_leaves.seekp(now.leave_address);
                file_leaves.write(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
                //!
                if (!i) {//如果删的是第一个数字，需要将父亲（可能比父亲还高级）的索引更改
                    tmp = now;
                    while (tmp.father != -1) {
                        file.seekg(tmp.father);
                        file.read(reinterpret_cast<char *>(&tmp), sizeof(node));
                        l = 0, r = tmp.length - 2;
                        int j;
                        find_it = 0;
                        while (l <= r) {
                            int mid = (l + r) >> 1;
                            if (!cpy(v_up.first, tmp.value[mid]) && !cpy(tmp.value[mid], v_up.first)) {
                                find_it = 1;
                                j = mid;
                                break;
                            }
                            if (cpy(v_up.first, tmp.value[mid])) r = mid - 1;
                            else l = mid + 1;
                        }
                        if (find_it) {
                            tmp.value[j] = now_l.value[0].first;
                            v_up = now_l.value[0];//!
                            file.seekp(tmp.address);
                            file.write(reinterpret_cast<char *>(&tmp), sizeof(node));
                            break;
                        }
                    }
                }
                if (now_l.length <= (N - 2) / 2) {
                    merge(now, v_up);
                }
                file.close();
                file_leaves.close();//!
                return;
            }
            file_leaves.close();
            file.close();
            throw int();
        }

        void modify(const Key &key, const T &v) {
            file.open(file_name);
            file.seekg(8);//!
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            now = root;
            while (!now.is_leave) {
                int l = 0, r = now.length - 2;
                while (l <= r) {
                    int mid = (l + r) >> 1;
                    if (cpy(key, now.value[mid])) r = mid - 1;
                    else l = mid + 1;
                }
                file.seekg(now.son[l]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            file_leaves.open(file_leaves_name);
            file_leaves.seekg(now.leave_address);
            file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
            int l = 0, r = now_l.length - 1;
            int i;
            bool find_it = 0;
            while (l <= r) {
                int mid = (l + r) >> 1;
                if (!cpy(key, now_l.value[mid].first) && !cpy(now_l.value[mid].first, key)) {
                    find_it = 1;
                    i = mid;
                    break;
                }
                if (cpy(key, now_l.value[mid].first)) r = mid - 1;
                else l = mid + 1;
            }
            if (find_it) {
                if (hs.count(now_l.value[i].first)) {
                    hs.erase(hs.find(now_l.value[i].first));
                }
                //!
                //!
                now_l.value[i].second = v;
                hs.insert(now_l.value[i]);
                file_leaves.seekp(now.leave_address);
                file_leaves.write(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
                file_leaves.close();
                file.close();
                return;
            }
            file_leaves.close();
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
            file.seekg(8);//!
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            root.length = 0;
            file.seekp(8);//!
            file.write(reinterpret_cast<char *>(&root), sizeof(node));
            file.close();
            //!
            file_leaves.open(file_leaves_name);
            long long x = 0;
            file_leaves.seekp(0);
            file_leaves.write(reinterpret_cast<char *>(&x), sizeof(long long));
            file_leaves.close();
            //!
        }

        sjtu::vector<value_type> traverse_val(const Key &l, const Key &r) {
            file.open(file_name);
            file.seekg(8);//!
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            if (!root.length) {
                file.close();
                throw int();
            }
            now = root;
            while (!now.is_leave) {
                int l1 = 0, r1 = now.length - 2;
                while (l1 <= r1) {
                    int mid = (l1 + r1) >> 1;
                    if (cpy(l, now.value[mid])) r1 = mid - 1;
                    else l1 = mid + 1;
                }
                file.seekg(now.son[l1]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            file_leaves.open(file_leaves_name);
            file_leaves.seekg(now.leave_address);
            file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
            file.close();
            sjtu::vector<value_type> vec_tmp;
            /*file_leaves.open(file_leaves_name);
            file_leaves.seekg(0);
            int x;
            file_leaves.read(reinterpret_cast<char *>(&x), sizeof(int));
            if (!x) {
                file_leaves.close();
                return vec_tmp;
            }
            file_leaves.seekg(x);
            file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));*/
            while (now_l.value[now_l.length - 1].first < l) {
                if (now_l.next == -1) {
                    file_leaves.close();
                    return vec_tmp;
                }
                file_leaves.seekg(now_l.next);
                file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
            }
            while (1) {
                for (int i = 0; i < now_l.length; i++) {
                    if (now_l.value[i].first >= l && now_l.value[i].first <= r) {
                        vec_tmp.push_back(now_l.value[i]);
                    } else if (now_l.value[i].first > r) {
                        file_leaves.close();
                        return vec_tmp;
                    }
                }
                if (now_l.next == -1) {
                    file_leaves.close();
                    return vec_tmp;
                }
                file_leaves.seekg(now_l.next);
                file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
            }
        }

        sjtu::vector<T> traverse(const Key &l, const Key &r) {
            file.open(file_name);
            file.seekg(8);//!
            file.read(reinterpret_cast<char *>(&root), sizeof(node));
            if (!root.length) {
                file.close();
                throw int();
            }
            now = root;
            while (!now.is_leave) {
                int l1 = 0, r1 = now.length - 2;
                while (l1 <= r1) {
                    int mid = (l1 + r1) >> 1;
                    if (cpy(l, now.value[mid])) r1 = mid - 1;
                    else l1 = mid + 1;
                }
                file.seekg(now.son[l1]);
                file.read(reinterpret_cast<char *>(&now), sizeof(node));
            }
            file_leaves.open(file_leaves_name);
            file_leaves.seekg(now.leave_address);
            file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
            file.close();
            sjtu::vector<T> vec_tmp;
            /*file_leaves.open(file_leaves_name);
            file_leaves.seekg(0);
            long long x;
            file_leaves.read(reinterpret_cast<char *>(&x), sizeof(long long));
            if (!x) {
                file_leaves.close();
                return vec_tmp;
            }
            file_leaves.seekg(x);
            file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));*/
            while (now_l.value[now_l.length - 1].first < l) {
                if (now_l.next == -1) {
                    file_leaves.close();
                    return vec_tmp;
                }
                file_leaves.seekg(now_l.next);
                file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
            }
            while (1) {
                for (int i = 0; i < now_l.length; i++) {
                    if (!(now_l.value[i].first < l) && !(r < now_l.value[i].first)) {
                        vec_tmp.push_back(now_l.value[i].second);
                    } else if (r < now_l.value[i].first) {
                        file_leaves.close();
                        return vec_tmp;
                    }
                }
                if (now_l.next == -1) {
                    file_leaves.close();
                    return vec_tmp;
                }
                file_leaves.seekg(now_l.next);
                file_leaves.read(reinterpret_cast<char *>(&now_l), sizeof(node_leaves));
            }
        }
    };
}

#endif //MAIN_MAP_CPP_BPT_H
