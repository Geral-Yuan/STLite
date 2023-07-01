#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev,*next;
        node():data(NULL),prev(NULL),next(NULL){}
        node(const T &x,node *p=NULL,node *n=NULL):prev(p),next(n){
            data=(T *) malloc(sizeof(T));
            new(data) T(x);
        }
        ~node(){
            if (data) {
                data->~T();
                free(data);
            }
        }
        /**
         * add data members and constructors & destructor
         */
    };

protected:
    /**
     * add data members for linked list as protected members
     */
     node *head,*tail;
     size_t length;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        cur->prev=pos->prev;
        cur->next=pos;
        pos->prev->next=cur;
        pos->prev=cur;
        length++;
        return cur;
    }
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        pos->next->prev=pos->prev;
        pos->prev->next=pos->next;
        length--;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
        friend class list;
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
         list<T> *list_ptr;
         node *pos;

    public:
        iterator(list<T> *list_p,node *p):list_ptr(list_p),pos(p){}
        iterator(const iterator &iter):list_ptr(iter.list_ptr),pos(iter.pos){}
        /**
         * iter++
         */
        iterator operator++(int) {
            if (pos==list_ptr->tail) throw invalid_iterator();
            node *p=pos;
            pos=pos->next;
            return iterator(list_ptr,p);
        }
        /**
         * ++iter
         */
        iterator & operator++() {
            if (pos==list_ptr->tail) throw invalid_iterator();
            pos=pos->next;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int) {
            if (pos==list_ptr->head->next) throw invalid_iterator();
            node *p=pos;
            pos=pos->prev;
            return iterator(list_ptr,p);
        }
        /**
         * --iter
         */
        iterator & operator--() {
            if (pos==list_ptr->head->next) throw invalid_iterator();
            pos=pos->prev;
            return *this;
        }
        /**
         * TODO *it
         * remember to throw if iterator is invalid
         */
        T & operator *() const {
            if (!(pos->data)) throw invalid_iterator();
            return *(pos->data);
        }
        /**
         * TODO it->field
         * remember to throw if iterator is invalid
         */
        T * operator ->() const {
            if (!(pos->data)) throw invalid_iterator();
            return pos->data;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const {
            return list_ptr==rhs.list_ptr&&pos==rhs.pos;
        }
        bool operator==(const const_iterator &rhs) const {
            return list_ptr==rhs.list_ptr&&pos==rhs.pos;
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
     * should be able to construct from an iterator.
     */
    class const_iterator {
        friend class list;
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        const list<T> *list_ptr;
        node *pos;

    public:
        const_iterator(const list<T> *list_p,node *p):list_ptr(list_p),pos(p){}
        const_iterator(const const_iterator &iter):list_ptr(iter.list_ptr),pos(iter.pos){}
        const_iterator(const iterator &iter):list_ptr(iter.list_ptr),pos(iter.pos){}
        /**
         * iter++
         */
        const_iterator operator++(int) {
            if (pos==list_ptr->tail) throw invalid_iterator();
            node *p=pos;
            pos=pos->next;
            return const_iterator(list_ptr,p);
        }
        /**
         * ++iter
         */
        const_iterator & operator++() {
            if (pos==list_ptr->tail) throw invalid_iterator();
            pos=pos->next;
            return *this;
        }
        /**
         * iter--
         */
        const_iterator operator--(int) {
            if (pos==list_ptr->head->next) throw invalid_iterator();
            node *p=pos;
            pos=pos->prev;
            return const_iterator(list_ptr,p);
        }
        /**
         * --iter
         */
        const_iterator & operator--() {
            if (pos==list_ptr->head->next) throw invalid_iterator();
            pos=pos->prev;
            return *this;
        }
        /**
         * TODO *it
         * remember to throw if iterator is invalid
         */
        const T & operator *() const {
            if (!(pos->data)) throw invalid_iterator();
            return *(pos->data);
        }
        /**
         * TODO it->field
         * remember to throw if iterator is invalid
         */
        const T * operator ->() const {
            if (!(pos->data)) throw invalid_iterator();
            return pos->data;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const {
            return list_ptr==rhs.list_ptr&&pos==rhs.pos;
        }
        bool operator==(const const_iterator &rhs) const {
            return list_ptr==rhs.list_ptr&&pos==rhs.pos;
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
     * At least two: default constructor, copy constructor
     */
    list() {
        head=new node;
        tail=new node;
        head->next=tail;
        tail->prev=head;
        length=0;
    }
    list(const list &other) {
        head=new node;
        tail=new node;
        node *p=other.head->next,*q=head;
        while (p!=other.tail){
            q->next=new node(*(p->data),q);
            q=q->next;
            p=p->next;
        }
        q->next=tail;
        tail->prev=q;
        length=other.length;
    }
    /**
     * TODO Destructor
     */
    virtual ~list() {
        node *p=head->next,*q;
        while (p!=tail){
            q=p;
            p=p->next;
            delete q;
        }
        delete head;
        delete tail;
    }
    /**
     * TODO Assignment operator
     */
    list &operator=(const list &other) {
        if (this==&other) return *this;
        clear();
        node *p=other.head->next,*q=head;
        while (p!=other.tail){
            q->next=new node(*(p->data),q);
            q=q->next;
            p=p->next;
        }
        q->next=tail;
        tail->prev=q;
        length=other.length;
        return *this;
    }
    /**
     * access the first / last element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const {
        if (length==0) throw container_is_empty();
        return *(head->next->data);
    }
    const T & back() const {
        if (length==0) throw container_is_empty();
        return *(tail->prev->data);
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        return iterator(this,head->next);
    }
    const_iterator cbegin() const {
        return const_iterator(this,head->next);
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return iterator(this,tail);
    }
    const_iterator cend() const {
        return const_iterator(this,tail);
    }
    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const {
        return length==0;
    }
    /**
     * returns the number of elements
     */
    virtual size_t size() const {
        return length;
    }

    /**
     * clears the contents
     */
    virtual void clear() {
        node *p=head->next,*q;
        while (p!=tail){
            q=p;
            p=p->next;
            delete q;
        }
        head->next=tail;
        tail->prev=head;
        length=0;
    }
    /**
     * insert value before pos (pos may be the end() iterator)
     * return an iterator pointing to the inserted value
     * throw if the iterator is invalid
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.pos!=tail&&!(pos.pos->data)||pos.list_ptr!=this) throw invalid_iterator();
        node *cur=new node(value);
        return iterator(this,insert(pos.pos,cur));
    }
    /**
     * remove the element at pos (the end() iterator is invalid)
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid
     */
    virtual iterator erase(iterator pos) {
        if (length==0) throw container_is_empty();
        if (!(pos.pos->data)||pos.list_ptr!=this) throw invalid_iterator();
        iterator iter(this,pos.pos->next);
        node *cur=erase(pos.pos);
        delete cur;
        return iter;
    }
    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        tail->prev=new node(value,tail->prev,tail);
        tail->prev->prev->next=tail->prev;
        length++;
    }
    /**
     * removes the last element
     * throw when the container is empty.
     */
    void pop_back() {
        if (length==0) throw container_is_empty();
        node *p=tail->prev;
        p->prev->next=tail;
        tail->prev=p->prev;
        delete p;
        length--;
    }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        head->next=new node(value,head,head->next);
        head->next->next->prev=head->next;
        length++;
    }
    /**
     * removes the first element.
     * throw when the container is empty.
     */
    void pop_front() {
        if (length==0) throw container_is_empty();
        node *p=head->next;
        p->next->prev=head;
        head->next=p->next;
        delete p;
        length--;
    }
    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        T* arr=(T *) malloc(sizeof(T)*length);
        size_t cnt=0;
        node *p=head->next;
        while (p!=tail){
            new(arr+cnt) T(*(p->data));
            p->data->~T();
            cnt++;
            p=p->next;
        }
        sjtu::sort<T>(arr,arr+cnt,std::less<T>());
        cnt=0;
        p=head->next;
        while (p!=tail){
            new(p->data) T(arr[cnt]);
            arr[cnt].~T();
            cnt++;
            p=p->next;
        }
        free(arr);
    }
    /**
     * merge two sorted lists into one (both in ascending order)
     * compare with operator< of T
     * container other becomes empty after the operation
     * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
     * the order of equivalent elements of *this and other does not change.
     * no elements are copied or moved
     */
    void merge(list &other) {
        node *p=head->next,*q=other.head->next,*tmp;
        while (p!=tail&&q!=other.tail){
            if (*(q->data)<*(p->data)){
                tmp=q->next;
                insert(p, other.erase(q));
                q=tmp;
            }else{
                p=p->next;
            }
        }
        while (q!=other.tail){
            tmp=q->next;
            insert(tail, other.erase(q));
            q=tmp;
        }
    }
    /**
     * reverse the order of the elements
     * no elements are copied or moved
     */
    void reverse() {
        node *p=head->next,*q=tail;
        while (p->next!=q){
            q=insert(q,erase(p));
            p=head->next;
        }
    }
    /**
     * remove all consecutive duplicate elements from the container
     * only the first element in each group of equal elements is left
     * use operator== of T to compare the elements.
     */
    void unique() {
        node *p=head->next;
        while (p!=tail&&p->next!=tail){
            if (*(p->next->data)==*(p->data)) {
                delete erase(p->next);
            }else {
                p=p->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
