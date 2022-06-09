/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"


namespace sjtu {

    struct my_true_type {
    };
    struct my_false_type {
    };

    template<typename T>
    struct my_type_traits {
        typedef typename T::iterator_assignable iterator_assignable;
        typedef typename T::value_type value_type;
        typedef typename T::iterator_default_constructor iterator_default_constructor;
        typedef typename T::iterator_copy_constructor iterator_copy_constructor;
        typedef typename T::iterator_trivial_destructor iterator_trivial_destructor;
    };


    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
        typedef my_true_type iterator_assignable;
        typedef my_true_type iterator_default_constructor;
        typedef my_true_type iterator_copy_constructor;
        typedef my_true_type iterator_trivial_destructor;
        size_t sz;

        struct Node {
            Node *son[2], *fa;
            value_type *value;
            bool col;

            Node() : value(nullptr), col(0), fa(nullptr) {
                son[0] = son[1] = nullptr;
            }

            Node(const Node &other) {
                son[0] = son[1] = fa = nullptr;
                value = new value_type(*other.value);
                col = other.col;
            }

            Node(const value_type &other) : col(0) {
                son[0] = son[1] = fa = nullptr;
                value = new value_type({other.first, other.second});
            }

            int Son() {
                return (son[0] != nullptr) + (son[1] != nullptr);
            }

            int RedSon() {
                return (son[0] != nullptr && son[0]->col == 1) + (son[1] != nullptr && son[1]->col == 1);
            }

            ~Node() {
                delete value;
            }
        };

        void _swap(Node *a, Node *b) {
            Node *tmp;
            tmp = a->son[0], a->son[0] = b->son[0], b->son[0] = tmp;
            tmp = a->son[1], a->son[1] = b->son[1], b->son[1] = tmp;
            tmp = a->fa, a->fa = b->fa, b->fa = tmp;
            bool t = a->col;
            a->col = b->col, b->col = t;
        }

        Node *Rt, *End, *Begin;

        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        int idx(const Node *now) const {
            return now == now->fa->son[1];
        }

        const Node *Find(const Node *other, bool fl) const {//Pre 0 Next 1
            if (other == End)return fl ? nullptr : End->fa;
            if (other == End->fa && fl == 1)return End;
            const Node *now = other;
            if (now->son[fl] != nullptr) {
                now = now->son[fl];
                while (now->son[!fl] != nullptr)now = now->son[!fl];
            } else {
                while (now != Rt && idx(now) == fl)now = now->fa;
                now = now->fa;
            }
            return now;
        }

        Node *find(Node *other, bool fl) { //Pre 0 Next 1
            if (other == End)return fl ? nullptr : End->fa;
            if (other == End->fa && fl == 1)return End;
            Node *now = other;
            if (now->son[fl] != nullptr) {
                now = now->son[fl];
                while (now->son[!fl] != nullptr)now = now->son[!fl];
            } else {
                while (now != Rt && idx(now) == fl)now = now->fa;
                now = now->fa;
            }
            return now;
        }

        class const_iterator;

        class iterator {
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            map *P;
            Node *node;
        public:
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
            using difference_type = std::ptrdiff_t;
            using value_type = map::value_type;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::output_iterator_tag;

            typedef my_true_type iterator_assignable;
            typedef my_true_type iterator_default_constructor;
            typedef my_true_type iterator_copy_constructor;
            typedef my_true_type iterator_trivial_destructor;
            // If you are interested in type_traits, toy_traits_test provides a place to
            // practice. But the method used in that test is old and rarely used, so you
            // may explore on your own.
            // Notice: you may add some code in here and class const_iterator and namespace sjtu to implement toy_traits_test,
            // this part is only for bonus.


            iterator() : P(nullptr), node(nullptr) {}

            iterator(map *P_, Node *node_) : P(P_), node(node_) {}

            iterator(const iterator &other) : P(other.Ask_P()), node(other.Ask_node()) {}

            /**
             * TODO iter++
             */
            map *Ask_P() const { return P; }

            Node *Ask_node() const { return node; }

            iterator operator++(int) {
                iterator ret = *this;
                Node *tmp = P->find(node, 1);
                if (tmp == nullptr)throw invalid_iterator();
                node = tmp;
                return ret;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                Node *tmp = P->find(node, 1);
                if (tmp == nullptr)throw invalid_iterator();
                node = tmp;
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator ret = *this;
                Node *tmp = P->find(node, 0);
                if (tmp == nullptr)throw invalid_iterator();
                node = tmp;
                return ret;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                Node *tmp = P->find(node, 0);
                if (tmp == nullptr)throw invalid_iterator();
                node = tmp;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                return *node->value;
            }

            bool operator==(const iterator &rhs) const {
                return P == rhs.Ask_P() && node == rhs.Ask_node();
            }

            bool operator==(const const_iterator &rhs) const {
                return P == rhs.Ask_P() && node == rhs.Ask_node();
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                return node->value;
            }
        };

        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            const map *P;
            const Node *node;
            // data members.
        public:

            using difference_type = std::ptrdiff_t;
            using value_type = map::value_type;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::output_iterator_tag;

            typedef my_false_type iterator_assignable;
            typedef my_true_type iterator_default_constructor;
            typedef my_true_type iterator_copy_constructor;
            typedef my_true_type iterator_trivial_destructor;


            const_iterator() : P(nullptr), node(nullptr) {}

            const_iterator(const map *P_, const Node *node_) : P(P_), node(node_) {}

            const_iterator(const const_iterator &other) : P(other.Ask_P()), node(other.Ask_node()) {}

            const_iterator(const iterator &other) : P(other.Ask_P()), node(other.Ask_node()) {}
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.

            const map *Ask_P() const { return P; }

            const Node *Ask_node() const { return node; }

            const_iterator operator++(int) {
                const_iterator ret = *this;
                const Node *tmp = P->Find(node, 1);
                if (tmp == nullptr)throw invalid_iterator();
                node = tmp;
                return ret;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                const Node *tmp = P->Find(node, 1);
                if (tmp == nullptr)throw invalid_iterator();
                node = tmp;
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator ret = *this;
                const Node *tmp = P->Find(node, 0);
                if (tmp == nullptr)throw invalid_iterator();
                node = tmp;
                return ret;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                const Node *tmp = P->Find(node, 0);
                if (tmp == nullptr)throw invalid_iterator();
                node = tmp;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            const value_type operator*() const {
                return *node->value;
            }

            bool operator==(const iterator &rhs) const {
                return P == rhs.Ask_P() && node == rhs.Ask_node();
            }

            bool operator==(const const_iterator &rhs) const {
                return P == rhs.Ask_P() && node == rhs.Ask_node();
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            const value_type *operator->() const noexcept {
                return node->value;
            }

        };

        /**
         * TODO two constructors
         */
        map() : Rt(nullptr), sz(0) {
            End = new Node;
            Begin = new Node;
        }

        Node *get_be(bool fl) {
            if (sz == 0)return nullptr;
            Node *now = Rt;
            while (now->son[fl] != nullptr)now = now->son[fl];
            return now;
        }

        void dfs_copy(Node *fa, Node *to, Node *fr) {
            to->fa = fa;
            for (int i = 0; i < 2; i++)
                if (fr->son[i] != nullptr) {
                    to->son[i] = new Node(*fr->son[i]);
                    dfs_copy(to, to->son[i], fr->son[i]);
                }
        }

        map(const map &other) : sz(other.sz), Rt(nullptr) {
            End = new Node;
            Begin = new Node;
            if (other.sz) {
                Rt = new Node(*other.Rt);
                dfs_copy(nullptr, Rt, other.Rt);
                End->fa = get_be(1);
                Begin->fa = get_be(0);
            }
        }

        /**
         * TODO assignment operator
         */
        void dfs_delete(Node *rt) {
            if (rt == nullptr)return;
            for (int i = 0; i < 2; i++)dfs_delete(rt->son[i]);
            delete rt;
        }

        map &operator=(const map &other) {
            if (&other == this)return *this;
            dfs_delete(Rt), End->fa = Begin->fa = nullptr;
            sz = other.sz;
            if (sz) {
                Rt = new Node(*other.Rt);
                dfs_copy(nullptr, Rt, other.Rt);
                End->fa = get_be(1);
                Begin->fa = get_be(0);
            }
            return *this;
        }

        /**
         * TODO Destructors
         */
        ~map() {
            dfs_delete(Rt);
            delete End;
            delete Begin;
        }

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        bool check(const Key &a, const Key &b) const {//a<b
            Compare tmp;
            return tmp(a, b);
        }

        bool eq(const Key &a, const Key &b) const {
            return !check(a, b) && !check(b, a);
        }

        T &at(const Key &key) {
            Node *now = Rt;
            while (now != nullptr) {
                if (eq(key, now->value->first))return now->value->second;
                else now = now->son[check(now->value->first, key)];
            }
            throw index_out_of_bound();
        }

        const T &at(const Key &key) const {
            Node *now = Rt;
            while (now != nullptr) {
                if (eq(key, now->value->first))return now->value->second;
                else now = now->son[check(now->value->first, key)];
            }
            throw index_out_of_bound();
        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            Node *now = Rt, *last = nullptr;
            while (now != nullptr) {
                if (eq(key, now->value->first))return now->value->second;
                last = now, now = now->son[check(now->value->first, key)];
            }
            return insert({key, T()}).first.Ask_node()->value->second;
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const { return at(key); }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return {this, sz == 0 ? End : Begin->fa};
        }

        const_iterator cbegin() const {
            return {this, sz == 0 ? End : Begin->fa};
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return {this, End};
        }

        const_iterator cend() const {
            return {this, End};
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const { return sz == 0; }

        /**
         * returns the number of elements.
         */
        size_t size() const { return sz; }

        long long Size() {
            return 1LL * sz * sizeof(Node);
        }

        /**
         * clears the contents
         */
        void clear() {
            Begin->fa = End->fa = nullptr;
            dfs_delete(Rt);
            Rt = nullptr;
            sz = 0;
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        void Rotate(Node *x) {
            int d = idx(x);
            Node *y = x->fa, *z = y->fa;
            if (z != nullptr)z->son[idx(y)] = x;
            else Rt = x;
            x->fa = z, y->fa = x;
            if (x->son[!d] != nullptr)x->son[!d]->fa = y;
            y->son[d] = x->son[!d];
            x->son[!d] = y;
            bool t = x->col;
            x->col = y->col, y->col = t;
        }

        void Up_insert(Node *now) {
            while (now->col + now->fa->col == 2) {
                Node *Fa = now->fa;
                int d = idx(Fa);
                if (Fa->fa->son[!d] != nullptr && Fa->fa->son[!d]->col == 1) {
                    if (Fa->fa == Rt) {
                        Fa->col = Fa->fa->son[!d]->col = 0;
                        break;
                    }
                    Fa->col = Fa->fa->son[!d]->col = 0;
                    Fa->fa->col = 1;
                    now = Fa->fa;
                } else {
                    if (idx(now) + d == 1)Rotate(now), Rotate(now);
                    else Rotate(Fa);
                    break;
                }
            }
        }

        pair<iterator, bool> insert(const value_type &value) {
            Node *now = Rt, *last = nullptr;
            while (now != nullptr) {
                if (eq(now->value->first, value.first))return {iterator(this, now), 0};
                last = now, now = now->son[check(now->value->first, value.first)];
            }
            Node *tmp = new Node(value);
            if (Rt == nullptr || check(End->fa->value->first, value.first))End->fa = tmp;
            if (Rt == nullptr || check(value.first, Begin->fa->value->first))Begin->fa = tmp;
            if ((++sz) == 1) Rt = tmp, Rt->col = 0;
            else {
                tmp->col = 1, tmp->fa = last;
                last->son[check(last->value->first, value.first)] = tmp;
                Up_insert(tmp);
            }
            return {iterator(this, tmp), 1};
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void Swap(Node *a, Node *b) {
            Node *f1 = a->fa, *f2 = b->fa;
            if (f2 == a) {
                if (f1 != nullptr)f1->son[idx(a)] = b;
                b->fa = b;
                _swap(a, b);
                b->son[b->son[1] == b] = a;
            } else {
                if (f2 != nullptr)f2->son[idx(b)] = a;
                if (f1 != nullptr)f1->son[idx(a)] = b;
                _swap(a, b);
            }
            for (int i = 0; i < 2; i++) {
                if (a->son[i] != nullptr)a->son[i]->fa = a;
                if (b->son[i] != nullptr)b->son[i]->fa = b;
            }
        }

        void Up_erase(Node *now, int d) {
            if (now->son[!d]->col == 1)Rotate(now->son[!d]);
            if (now->son[!d]->RedSon() == 0) {
                int be = now->col;
                now->son[!d]->col = 1, now->col = 0;
                if (be == 0 && now != Rt)Up_erase(now->fa, idx(now));
            } else if (now->son[!d]->son[d] != nullptr && now->son[!d]->son[d]->col == 1) {
                Node *p = now->son[!d]->son[d];
                Rotate(p), Rotate(p);
                p->son[!d]->col = 0;
            } else Rotate(now->son[!d]), now->fa->son[!d]->col = 0;
        }

        void Delete_node(Node *now) {
            if (now->Son() == 1) {
                bool fl = now->son[0] == nullptr;
                Node *del = now->son[fl];
                if (now == Rt)Rt = del;
                Swap(now, del);
                delete now;
                del->son[fl] = nullptr;
            } else if (now->col == 0) {
                if (now != Rt) {
                    Node *st = now->fa;
                    int dir = idx(now);
                    now->fa->son[idx(now)] = nullptr;
                    Up_erase(st, dir);
                } else Rt = nullptr;
                delete now;
            } else {
                if (now != Rt)now->fa->son[idx(now)] = nullptr;
                delete now;
            }
        }

        void erase(iterator pos) {
            if (pos.Ask_P() != this || pos.Ask_node() == End)throw index_out_of_bound();
            Node *now = Rt;
            while (now != nullptr) {
                if (eq(now->value->first, pos.Ask_node()->value->first)) {
                    if (now->Son() == 2) {
                        Node *del = find(now, 1);
                        if (now == Rt)Rt = del;
                        Swap(now, del);
                    }
                    Delete_node(now);
                    sz--;
                    End->fa = get_be(1);
                    Begin->fa = get_be(0);
                    return;
                }
                now = now->son[check(now->value->first, pos.Ask_node()->value->first)];
            }
            throw index_out_of_bound();
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            Node *now = Rt;
            while (now != nullptr) {
                if (eq(key, now->value->first))return 1;
                now = now->son[check(now->value->first, key)];
            }
            return 0;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            Node *now = Rt;
            while (now != nullptr) {
                if (eq(now->value->first, key))return iterator(this, now);
                now = now->son[check(now->value->first, key)];
            }
            return {this, End};
        }

        const_iterator find(const Key &key) const {
            const Node *now = Rt;
            while (now != nullptr) {
                if (eq(now->value->first, key))return {this, now};
                now = now->son[check(now->value->first, key)];
            }
            return const_iterator(this, End);
        }
    };

}

#endif
