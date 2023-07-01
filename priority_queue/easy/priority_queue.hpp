#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    size_t crtSize;
    T *data;
    size_t capacity;

    void doubleSpace(){
        T* tmp=data;
        capacity*=2;
        data=(T*) malloc(sizeof(T)*capacity);
        for (int i=1;i<=crtSize;i++) {
            new(data+i) T(tmp[i]);
            tmp[i].~T();
        }
        free(tmp);
    }
    void percolateDown(int hole){
        int child;
        T tmp=data[hole];
        for (;hole*2<=crtSize;hole=child){
            child=hole*2;
            if (child!=crtSize&&Compare()(data[child],data[child+1]))
                child++;
            if (Compare()(tmp,data[child]))
                data[hole]=data[child];
            else
                break;
        }
        data[hole]=tmp;
    }
public:
	/**
	 * TODO constructors
	 */
    priority_queue(int cap=100):capacity(cap) {
        data=(T*) malloc(sizeof(T)*capacity);
        crtSize=0;
    }
    priority_queue(const priority_queue &other):capacity(other.capacity),crtSize(other.crtSize) {
        data=(T*) malloc(sizeof(T)*capacity);
        for (int i=1;i<=crtSize;i++)
            new(data+i) T(other.data[i]);
    }
	/**
	 * TODO deconstructor
	 */
    ~priority_queue() {
        for (int i=1;i<=crtSize;i++)
            data[i].~T();
        free(data);
    }
	/**
	 * TODO Assignment operator
	 */
    priority_queue &operator=(const priority_queue &other) {
        if (this==&other) return *this;
        for (int i=1;i<=crtSize;i++)
            data[i].~T();
        free(data);
        capacity=other.capacity;
        crtSize=other.crtSize;
        data=(T*) malloc(sizeof(T)*capacity);
        for (int i=1;i<=crtSize;i++)
            new(data+i) T(other.data[i]);
        return *this;
    }
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
    const T & top() const {
        if (crtSize==0) throw container_is_empty();
        return data[1];
    }
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
    void push(const T &e) {
        if (crtSize==capacity-1) doubleSpace();
        int hole=++crtSize;
        new(data+hole) T(e);
        for (;hole>1&&Compare()(data[hole/2],e);hole/=2)
            data[hole]=data[hole/2];
        data[hole]=e;
    }
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
    void pop() {
        if (crtSize==0) throw container_is_empty();
        data[1]=data[crtSize];
        data[crtSize].~T();
        crtSize--;
        percolateDown(1);
    }
	/**
	 * return the number of the elements.
	 */
    size_t size() const { return crtSize;}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
    bool empty() const { return crtSize==0;}
};

}

#endif