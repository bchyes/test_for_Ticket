#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector {
    public:
        /**
         * TODO
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
        int length, maxn;
        T **array;

        class const_iterator;

        class iterator {
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            T **point;
            T **beg;
            vector<T> *v;
        public:
            iterator(T **arr, T **beg_, vector<T> *v_) {
                point = arr;
                beg = beg_;
                v = v_;
            }

            iterator &operator=(const iterator &rhs) {
                point = rhs.point;
                return *this;
            }

            iterator &operator=(const T *&rhs) {
                point = rhs;
                return *this;
            }

            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                //TODO
                iterator tmp(point + n, beg, v);
                return tmp;
            }

            iterator operator-(const int &n) const {
                //TODO
                iterator tmp(point - n, beg, v);
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                //TODO
                if (this != &rhs) throw invalid_iterator();
                return abs(point - rhs.point);
            }

            iterator &operator+=(const int &n) {
                //TODO
                for (int i = 1; i <= n; i++)
                    point++;
                return *this;
            }

            iterator &operator-=(const int &n) {
                //TODO
                for (int i = 1; i <= n; i++)
                    point--;
                return *this;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                T *tmp = point;
                point++;
                return tmp;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                point++;
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                T *tmp = point;
                point--;
                return tmp;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                point--;
                return *this;
            }

            /**
             * TODO *it
             */
            T &operator*() const {
                return **point;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return point == rhs.point && v == rhs.v;
            }

            bool operator==(const const_iterator &rhs) const {
                return point == rhs.point && v == rhs.v;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return point != rhs.point || v != rhs.v;
            }

            bool operator!=(const const_iterator &rhs) const {
                return point != rhs.point || v != rhs.v;
            }

            operator int() {
                return (point - beg);
            }
        };

        /**
         * TODO
         * has same function as iterator, just for a const object.
         */
        class const_iterator {
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            T *const *point;
            T *const *beg;
            const vector<T> *v;
        public:
            const_iterator(T *const *arr, T *const *beg_, const vector<T> *v_) {
                point = arr;
                beg = beg_;
                v = v_;
            }

            const_iterator &operator=(const const_iterator &rhs) {
                point = rhs.point;
                return *this;
            }

            const_iterator &operator=(const T *&rhs) {
                point = rhs;
                return *this;
            }

            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator operator+(const int &n) const {
                //TODO
                iterator tmp(point + n, beg, v);
                return tmp;
            }

            const_iterator operator-(const int &n) const {
                //TODO
                iterator tmp(point - n, beg, v);
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                //TODO
                if (this != &rhs) throw invalid_iterator();
                return abs(point - rhs.point);
            }

            const_iterator &operator+=(const int &n) {
                //TODO
                for (int i = 1; i <= n; i++)
                    point++;
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                //TODO
                for (int i = 1; i <= n; i++)
                    point--;
                return *this;
            }

            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                const T *tmp = point;
                point++;
                return tmp;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                point++;
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                T *tmp = point;
                point--;
                return tmp;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                point--;
                return *this;
            }

            /**
             * TODO *it
             */
            const T &operator*() const {
                return **point;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return point == rhs.point && v == rhs.v;
            }

            bool operator==(const const_iterator &rhs) const {
                return point == rhs.point && v == rhs.v;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return point != rhs.point || v != rhs.v;
            }

            bool operator!=(const const_iterator &rhs) const {
                return point != rhs.point || v != rhs.v;
            }

            explicit operator int() {
                return (point - beg);
            }
        };

        /*iterator pointer = array;

        const_iterator cpointer = array;*/

        void doublesize() {
            T **tmp = new T *[maxn * 2];
            for (int i = 0; i < maxn; i++)
                tmp[i] = array[i];
            delete[]array;
            array = tmp;
            maxn *= 2;
        }

        /**
         * TODO Constructs
         * Atleast two: default constructor, copy constructor
         */
        vector() {
            array = new T *[10];
            maxn = 10;
            length = 0;
        }

        vector(const vector &other) {
            length = other.length;
            maxn = other.maxn;
            array = new T *[maxn];
            for (int i = 0; i < length; i++) {
                array[i] = new T(*other.array[i]);
            }
        }

        /**
         * TODO Destructor
         */
        ~vector() {
            for (int i = 0; i < length; i++)
                delete array[i];
            delete[]array;
        }

        /**
         * TODO Assignment operator
         */
        vector &operator=(const vector &other) {
            if (this == &other) return *this;
            for (int i = 0; i < length; i++)
                delete array[i];
            delete[]array;
            length = other.length;
            maxn = other.maxn;
            array = new T *[maxn];
            for (int i = 0; i < length; i++) {
                array[i] = new T(*other.array[i]);
            }
            /*pointer = other.pointer;
            cpointer = other.cpointer;*/
            return *this;
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T &at(const size_t &pos) {
            if (pos >= length || pos < 0) throw index_out_of_bound();
            return *array[pos];
        }

        const T &at(const size_t &pos) const {
            if (pos >= length || pos < 0) throw index_out_of_bound();
            return *array[pos];
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            if (pos >= length || pos < 0) throw index_out_of_bound();
            return *array[pos];
        }

        const T &operator[](const size_t &pos) const {
            if (pos >= length || pos < 0) throw index_out_of_bound();
            return *array[pos];
        }

        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T &front() const {
            if (!length) throw container_is_empty();
            return *array[0];
        }

        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T &back() const {
            if (!length) throw container_is_empty();
            return *array[length - 1];
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            /*pointer = array;*/

            return iterator(array, array, this);
        }

        const_iterator cbegin() const {
            /*cpointer = array;*/
            return const_iterator(array, array, this);
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            iterator pointer(array + length, array, this);
            /*for (int i = 1; i < length; i++)
                pointer++;*/
            return pointer;
        }

        const_iterator cend() const {
            const_iterator cpointer(array + length, array, this);
            /*for (int i = 1; i < length; i++)
                cpointer++;*/
            return cpointer;
        }

        /**
         * checks whether the container is empty
         */
        bool empty() const {
            return length == 0;
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return length;
        }

        /**
         * clears the contents
         */
        void clear() {
            length = 0;
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            if (length + 1 >= maxn) doublesize();
            if (int(pos) == length) {
                array[length] = new T(value);
            } else {
                array[length] = new T(*array[length - 1]);
                for (int i = length - 2; i >= int(pos); i--)
                    *array[i + 1] = *array[i];
                *array[int(pos)] = value;
            }
            length++;
            iterator pointer(&array[int(pos)], array, this);
            return pointer;
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if (length + 1 >= maxn) doublesize();
            if (ind == length) {
                array[length] = new T(value);
            } else {
                array[length] = new T(*array[length - 1]);
                for (int i = length - 2; i >= ind; i--)
                    *array[i + 1] = *array[i];
                *array[ind] = value;
            }
            length++;
            iterator pointer(&array[ind], array, this);
            return pointer;
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            for (int i = int(pos); i < length - 1; i++)
                *array[i] = *array[i + 1];
            delete array[length - 1];
            length--;
            iterator pointer(&array[int(pos)], array, this);
            return pointer;
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            for (int i = ind; i < length - 1; i++)
                *array[i] = *array[i + 1];
            delete array[length - 1];
            length--;
            iterator pointer(&array[ind], array, this);
            return pointer;
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            if (length + 1 >= maxn) doublesize();
            array[length++] = new T(value);
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if (!length) throw container_is_empty();
            delete array[length - 1];
            length--;
        }
    };


}

#endif