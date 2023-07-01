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
private:
    T *data;
    size_t length;
    size_t capacity;
    void doubleSpace(){
        T *tmp=data;
        capacity*=2;
        data=(T *) malloc(sizeof(T)*capacity);
        for (int i=0;i<length;i++){
            new(data+i) T(tmp[i]);
            tmp[i].~T();
        }
        free(tmp);
    }
public:
    /**
     * TODO
     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
    /**
     * you can see RandomAccessIterator at CppReference for help.
     */
    class const_iterator;
    class iterator {
        friend class vector::const_iterator;
    private:
        vector<T> *vec_head;
        int index;
        /**
         * TODO add data members
         *   just add whatever you want.
         */
    public:
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator(vector<T> *p, int idx=0){
            vec_head=p;
            index=idx;
        }
        iterator operator+(const int &n) const {
            return iterator(vec_head,index+n);
            //TODO
        }
        iterator operator-(const int &n) const {
            return iterator(vec_head,index-n);
            //TODO
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
        int operator-(const iterator &rhs) const {
            if (vec_head!=rhs.vec_head) throw invalid_iterator();
            return index-rhs.index;
            //TODO
        }
        iterator& operator+=(const int &n) {
            index+=n;
            return *this;
            //TODO
        }
        iterator& operator-=(const int &n) {
            index-=n;
            return *this;
            //TODO
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            iterator tmp=*this;
            index++;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        iterator& operator++() {
            index++;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            iterator tmp=*this;
            index--;
            return tmp;
        }
        /**
         * TODO --iter
         */
        iterator& operator--() {
            index--;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const{
            return vec_head->data[index];
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator==(const iterator &rhs) const {
            return vec_head==rhs.vec_head&&index==rhs.index;
        }
        bool operator==(const const_iterator &rhs) const {
            return vec_head==rhs.vec_head&&index==rhs.index;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return !((*this)==rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !((*this)==rhs);
        }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
        friend class vector::iterator;
    private:
        const vector<T> *vec_head;
        int index;
        /**
         * TODO add data members
         *   just add whatever you want.
         */
    public:
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        const_iterator(const vector<T> *p, int idx=0){
            vec_head=p;
            index=idx;
        }
        const_iterator operator+(const int &n) const {
            return const_iterator(vec_head,index+n);
            //TODO
        }
        const_iterator operator-(const int &n) const {
            return const_iterator(vec_head,index-n);
            //TODO
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
        int operator-(const const_iterator &rhs) const {
            if (vec_head!=rhs.vec_head) throw invalid_iterator();
            return index-rhs.index;
            //TODO
        }
        const_iterator& operator+=(const int &n) {
            index+=n;
            return *this;
            //TODO
        }
        const_iterator& operator-=(const int &n) {
            index-=n;
            return *this;
            //TODO
        }
        /**
         * TODO iter++
         */
        const_iterator operator++(int) {
            const_iterator tmp=*this;
            index++;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        const_iterator& operator++() {
            index++;
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            const_iterator tmp=*this;
            index--;
            return tmp;
        }
        /**
         * TODO --iter
         */
        const_iterator& operator--() {
            index--;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const{
            return vec_head->data[index];
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator==(const iterator &rhs) const {
            return vec_head==rhs.vec_head&&index==rhs.index;
        }
        bool operator==(const const_iterator &rhs) const {
            return vec_head==rhs.vec_head&&index==rhs.index;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return !((*this)==rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !((*this)==rhs);
        }
    };
    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    vector():length(0),capacity(16) {
        data=(T *) malloc(sizeof(T)*capacity);
    }
    vector(const vector &other):length(other.length),capacity(other.capacity) {
        data=(T *) malloc(sizeof(T)*capacity);
        for (int i=0;i<length;i++)
            new(data+i) T(other.data[i]);
    }
    /**
     * TODO Destructor
     */
    ~vector() {
        for (int i=0;i<length;i++)
            data[i].~T();
        free(data);
    }
    /**
     * TODO Assignment operator
     */
    vector &operator=(const vector &other) {
        if (this==&other) return *this;
        for (int i=0;i<length;i++)
            data[i].~T();
        free(data);
        length=other.length;
        capacity=other.capacity;
        data=(T *) malloc(sizeof(T)*capacity);
        for (int i=0;i<length;i++)
            new(data+i) T(other.data[i]);
        return *this;
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T & at(const size_t &pos) {
        if (pos<0||pos>=length) throw index_out_of_bound();
        return data[pos];
    }
    const T & at(const size_t &pos) const {
        if (pos<0||pos>=length) throw index_out_of_bound();
        return data[pos];
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T & operator[](const size_t &pos) {
        if (pos<0||pos>=length) throw index_out_of_bound();
        return data[pos];
    }
    const T & operator[](const size_t &pos) const {
        if (pos<0||pos>=length) throw index_out_of_bound();
        return data[pos];
    }
    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T & front() const {
        if (length==0) throw container_is_empty();
        return data[0];
    }
    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T & back() const {
        if (length==0) throw container_is_empty();
        return data[length-1];
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {return iterator(this,0);}
    const_iterator cbegin() const {return const_iterator(this,0);}
    /**
     * returns an iterator to the end.
     */
    iterator end() {return iterator(this,length);}
    const_iterator cend() const {return const_iterator(this,length);}
    /**
     * checks whether the container is empty
     */
    bool empty() const {return length==0;}
    /**
     * returns the number of elements
     */
    size_t size() const {return length;}
    /**
     * clears the contents
     */
    void clear() {
        for (int i=0;i<length;i++)
            data[i].~T();
        length=0;
    }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value) {
        size_t ind=pos-begin();
        if (length==capacity) doubleSpace();
        for (int i=length;i>ind;i--)
            data[i]=data[i-1];
        new(data+ind) T(value);
        length++;
        return pos;
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value) {
        if (ind>length) throw index_out_of_bound();
        if (length==capacity) doubleSpace();
        for (int i=length;i>ind;i--)
            data[i]=data[i-1];
        new(data+ind) T(value);
        length++;
        return iterator(this,ind);
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos) {
        size_t ind=pos-begin();
        data[ind].~T();
        for (int i=ind;i<length-1;i++)
            data[i]=data[i+1];
        length--;
        return pos;
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
        if (ind>=length) throw index_out_of_bound();
        data[ind].~T();
        for (int i=ind;i<length-1;i++)
            data[i]=data[i+1];
        length--;
        return iterator(this,ind);
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
        if (length==capacity) doubleSpace();
        new(data+length) T(value);
        length++;
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
        if (length==0) throw container_is_empty();
        data[length-1].~T();
        length--;
    }
};


}

#endif
