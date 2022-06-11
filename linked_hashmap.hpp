/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_LINKEDHASHMAP_HPP
#define SJTU_LINKEDHASHMAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
    /**
     * In linked_hashmap, iteration ordering is differ from map,
     * which is the order in which keys were inserted into the map.
     * You should maintain a doubly-linked list running through all
     * of its entries to keep the correct iteration order.
     *
     * Note that insertion order is not affected if a key is re-inserted
     * into the map.
     */

    template<
            class Key,
            class T,
            class Hash = std::hash<Key>,
            class Equal = std::equal_to<Key>
    >
    class linked_hashmap {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::linked_hashmap as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;

        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = linked_hashmap.begin(); --it;
         *       or it = linked_hashmap.end(); ++end();
         */
        struct node {
            node *pre = NULL, *next = NULL, *fa = NULL, *son = NULL;
            node *last = NULL;
            value_type *value = NULL;

            node() {}

            node(const value_type &val) { value = new value_type(val); }

            ~node() {
                delete value;
            }
        };

        node **array = NULL;
        node *head = NULL;
        node *tail = NULL;
        int p[7] = {13, 131, 1327, 16127, 131057, 1398269, 13466917};
        int p_ = 0;
        Hash h;
        Equal e;
        int size_ = 0;

        class const_iterator;

        class iterator {
            friend linked_hashmap<Key, T, Hash, Equal>;
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            linked_hashmap<Key, T, Hash, Equal> *mp;
            node *head = NULL;
            node *tail = NULL;
            node *now = NULL;

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

            /*using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;*/

            iterator() {
                // TODO
            }

            iterator(linked_hashmap<Key, T, Hash, Equal> *mp_, node *head_, node *tail_, node *now_) {
                mp = mp_;
                head = head_;
                tail = tail_;
                now = now_;
            }

            iterator(const iterator &other) {
                // TODO
                mp = other.mp;
                head = other.head;
                tail = other.tail;
                now = other.now;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                if (now == tail) throw index_out_of_bound();
                node *tmp = now;
                now = now->next;
                return iterator(mp, head, tail, tmp);
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                if (now == tail) throw index_out_of_bound();
                now = now->next;
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                if (now->pre == head) throw index_out_of_bound();
                node *tmp = now;
                now = now->pre;
                return iterator(mp, head, tail, tmp);
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                if (now->pre == head) throw index_out_of_bound();
                now = now->pre;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                return *now->value;
            }

            bool operator==(const iterator &rhs) const {
                return mp == rhs.mp && now == rhs.now;
            }

            bool operator==(const const_iterator &rhs) const {
                return mp == rhs.mp && now == rhs.now;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return mp != rhs.mp || now != rhs.now;
            }

            bool operator!=(const const_iterator &rhs) const {
                return mp != rhs.mp || now != rhs.now;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                return now->value;
            }
        };

        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
            friend iterator;
            friend linked_hashmap<Key, T, Hash, Equal>;
        private:
            // data members.
            const linked_hashmap<Key, T, Hash, Equal> *mp;
            const node *head = NULL;
            const node *tail = NULL;
            const node *now = NULL;
        public:
            const_iterator() {
                // TODO
            }

            const_iterator(const linked_hashmap<Key, T, Hash, Equal> *mp_, const node *head_, const node *tail_,
                           const node *now_) {
                mp = mp_;
                head = head_;
                tail = tail_;
                now = now_;
            }

            const_iterator(const const_iterator &other) {
                // TODO
                mp = other.mp;
                head = other.head;
                tail = other.tail;
                now = other.now;
            }

            const_iterator(const iterator &other) {
                // TODO
                mp = other.mp;
                head = other.head;
                tail = other.tail;
                now = other.now;
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
/**
             * TODO iter++
             */
            const_iterator operator++(int) {
                if (now == tail) throw index_out_of_bound();
                const node *tmp = now;
                now = now->next;
                return const_iterator(mp, head, tail, tmp);
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                if (now == tail) throw index_out_of_bound();
                now = now->next;
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                if (now->pre == head) throw index_out_of_bound();
                const node *tmp = now;
                now = now->pre;
                return const_iterator(mp, head, tail, tmp);
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                if (now->pre == head) throw index_out_of_bound();
                now = now->pre;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                return *now->value;
            }

            bool operator==(const iterator &rhs) const {
                return mp == rhs.mp && now == rhs.now;
            }

            bool operator==(const const_iterator &rhs) const {
                return mp == rhs.mp && now == rhs.now;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return mp != rhs.mp || now != rhs.now;
            }

            bool operator!=(const const_iterator &rhs) const {
                return mp != rhs.mp || now != rhs.now;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            const value_type *operator->() const noexcept {
                return now->value;
            }
        };

        /**
         * TODO two constructors
         */
        linked_hashmap() {
            array = new node *[p[p_]];
            head = new node;
            tail = new node;
            head->next = tail;
            tail->pre = head;
            for (int i = 0; i < p[p_]; i++) array[i] = NULL;
        }

        linked_hashmap(const linked_hashmap &other) {
            p_ = other.p_;
            array = new node *[p[p_]];
            head = new node;
            tail = new node;
            head->next = tail;
            tail->pre = head;
            for (int i = 0; i < p[p_]; i++) array[i] = NULL;
            for (const_iterator it = other.cbegin(); it != other.cend(); it++) {
                insert(*it);
            }
        }

        /**
         * TODO assignment operator
         */
        linked_hashmap &operator=(const linked_hashmap &other) {
            if (this == &other) return *this;
            clear();
            delete[]array;
            p_ = other.p_;
            array = new node *[p[p_]];
            if (head == NULL)
                head = new node;
            if (tail == NULL)
                tail = new node;
            head->next = tail;
            tail->pre = head;
            for (int i = 0; i < p[p_]; i++) array[i] = NULL;
            for (const_iterator it = other.cbegin(); it != other.cend(); it++) {
                insert(*it);
            }
            return *this;
        }

        /**
         * TODO Destructors
         */
        ~linked_hashmap() {
            clear();
            delete[]array;
            delete head;
            delete tail;
        }

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            iterator it = find(key);
            if (it == end()) {
                throw index_out_of_bound();
            } else return it.now->value->second;
        }

        const T &at(const Key &key) const {
            const_iterator it = find(key);
            if (it == cend()) {
                throw index_out_of_bound();
            } else return it.now->value->second;
        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            iterator it = find(key);
            if (it == end()) {
                pair<iterator, bool> add = insert(pair<Key, T>(key, T()));
                return add.first.now->value->second;
            } else return it.now->value->second;
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            const_iterator it = find(key);
            if (it == cend()) {
                throw index_out_of_bound();
            } else return it.now->value->second;
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator(this, head, tail, head->next);
        }

        const_iterator cbegin() const {
            return const_iterator(this, head, tail, head->next);
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(this, head, tail, tail);
        }

        const_iterator cend() const {
            return const_iterator(this, head, tail, tail);
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return !size_;
        }

        /**
         * returns the number of elements.
         */
        size_t size() const {
            return size_;
        }

        /**
         * clears the contents
         */
        void clear() {
            iterator it = begin();
            while (it != end()) {
                erase(it++);
            }
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            iterator it = find(value.first);
            if (it != end()) return pair<iterator, bool>(it, false);
            int hash = h(value.first) % p[p_];
            if (array[hash] == NULL) {
                array[hash] = new node;
                array[hash]->son = new node(value);
                array[hash]->son->fa = array[hash];
                array[hash]->last = array[hash]->son;
            } else {
                array[hash]->last->son = new node(value);
                array[hash]->last->son->fa = array[hash]->last;
                array[hash]->last = array[hash]->last->son;
            }
            node *tmp = tail->pre;
            tmp->next = array[hash]->last;
            array[hash]->last->pre = tmp;
            array[hash]->last->next = tail;
            tail->pre = array[hash]->last;
            size_++;
            if (p_ <= 5 && size_ >= p[p_]) {
                for (int i = 0; i < p[p_]; i++)
                    delete array[i];
                p_++;
                delete[]array;
                array = new node *[p[p_]];
                for (int i = 0; i < p[p_]; i++) array[i] = NULL;
                node *tmp_ = head->next;
                while (tmp_ != tail) {
                    tmp_->fa = tmp_->son = NULL;
                    tmp_ = tmp_->next;
                }
                tmp_ = head->next;
                while (tmp_ != tail) {
                    int hash_ = h(tmp_->value->first) % p[p_];
                    if (array[hash_] == NULL) {
                        array[hash_] = new node;
                        array[hash_]->son = tmp_;
                        array[hash_]->son->fa = array[hash_];
                        array[hash_]->last = array[hash_]->son;
                    } else {
                        array[hash_]->last->son = tmp_;
                        array[hash_]->last->son->fa = array[hash_]->last;
                        array[hash_]->last = array[hash_]->last->son;
                    }
                    tmp_ = tmp_->next;
                }
                hash = h(value.first) % p[p_];
            }
            return pair<iterator, bool>(iterator(this, head, tail, array[hash]->last), true);
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos.mp != this) throw invalid_iterator();
            if (pos.now == tail || pos.now == head)throw index_out_of_bound();
            size_--;
            pos.now->pre->next = pos.now->next;
            pos.now->next->pre = pos.now->pre;
            int hash = h(pos.now->value->first) % p[p_];
            if (pos.now->son == NULL && pos.now->fa == array[hash]) {
                delete pos.now;
                delete array[hash];
                array[hash] = NULL;
                return;
            }
            if (pos.now->son != NULL) {
                pos.now->son->fa = pos.now->fa;
            } else array[hash]->last = pos.now->fa;
            pos.now->fa->son = pos.now->son;
            delete pos.now;
            if (p_ && size_ <= p[p_] / 20) {
                for (int i = 0; i < p[p_]; i++)
                    delete array[i];
                p_--;
                delete[]array;
                array = new node *[p[p_]];
                for (int i = 0; i < p[p_]; i++)
                    array[i] = NULL;
                node *tmp_ = head->next;
                while (tmp_ != tail) {
                    tmp_->fa = tmp_->son = NULL;
                    tmp_ = tmp_->next;
                }
                tmp_ = head->next;
                while (tmp_ != tail) {
                    int hash_ = h(tmp_->value->first) % p[p_];
                    if (array[hash_] == NULL) {
                        array[hash_] = new node;
                        array[hash_]->son = tmp_;
                        array[hash_]->son->fa = array[hash_];
                        array[hash_]->last = array[hash_]->son;
                    } else {
                        array[hash_]->last->son = tmp_;
                        array[hash_]->last->son->fa = array[hash_]->last;
                        array[hash_]->last = array[hash_]->last->son;
                    }
                    tmp_ = tmp_->next;
                }
            }
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         */
        size_t count(const Key &key) const {
            const_iterator it = find(key);
            if (it == cend()) return 0;
            else return 1;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            int hash = h(key) % p[p_];
            node *now = array[hash];
            if (now == NULL) return iterator(this, head, tail, tail);
            while (now->son != NULL) {
                if (e(key, now->son->value->first)) {
                    return iterator(this, head, tail, now->son);
                }
                now = now->son;
            }
            return iterator(this, head, tail, tail);
        }

        const_iterator find(const Key &key) const {
            int hash = h(key) % p[p_];
            node *now = array[hash];
            if (now == NULL) return const_iterator(this, head, tail, tail);
            while (now->son != NULL) {
                if (e(key, now->son->value->first)) {
                    return const_iterator(this, head, tail, now->son);
                }
                now = now->son;
            }
            return const_iterator(this, head, tail, tail);
        }
    };

}

#endif
