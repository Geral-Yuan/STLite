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
    struct node{
        T data;
        node *left,*right;
        size_t npl;
        node(T x,node *l=nullptr,node *r=nullptr):data(x),left(l),right(r){
            if (l&&r){
                npl=(l->npl<r->npl?l->npl:r->npl)+1;
            }else{
                npl=0;
            }
        }
    };

    node *root;
    size_t crtSize;
    void clear(node *&_root){
        if (!_root) return;
        clear(_root->left);
        clear(_root->right);
        delete _root;
        _root=nullptr;
    }
    void copy(node *&lhs,node *rhs){
        if (!rhs) return;
        lhs=new node(rhs->data);
        copy(lhs->left,rhs->left);
        copy(lhs->right,rhs->right);
        lhs->npl=rhs->npl;
    }
    node *mergeNode(node *x,node *y){
        if (!x) return y;
        if (!y) return x;
        if (Compare()(x->data,y->data)){
            std::swap(x,y);
        }
        x->right=mergeNode(x->right,y);
        if ((!x->left)||x->left->npl<x->right->npl){
            std::swap(x->left,x->right);
        }
        if (x->right){
            x->npl=x->right->npl+1;
        }else{
            x->npl=0;
        }
        return x;
    }
public:
	/**
	 * TODO constructors
	 */
    priority_queue() {
        root=nullptr;
        crtSize=0;
    }
    priority_queue(const priority_queue &other){
        copy(root,other.root);
        crtSize=other.crtSize;
    }
	/**
	 * TODO deconstructor
	 */
    ~priority_queue() {
        clear(root);
    }
	/**
	 * TODO Assignment operator
	 */
    priority_queue &operator=(const priority_queue &other) {
        if (this==&other) return *this;
        clear(root);
        copy(root,other.root);
        crtSize=other.crtSize;
        return *this;
    }
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
    const T & top() const {
        if (crtSize==0) throw container_is_empty();
        return root->data;
    }
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
    void push(const T &e) {
        node *p=new node(e);
        root=mergeNode(root,p);
        crtSize++;
    }
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
    void pop() {
        if (crtSize==0) throw container_is_empty();
        node *tmp=root;
        root=mergeNode(root->left,root->right);
        delete tmp;
        crtSize--;
    }
	/**
	 * return the number of the elements.
	 */
    size_t size() const {
        return crtSize;
    }
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
    bool empty() const {
        return crtSize==0;
    }
	/**
	 * merge two priority_queues with at least O(logn) complexity.
	 * clear the other priority_queue.
	 */
    void merge(priority_queue &other) {
        root=mergeNode(root,other.root);
        crtSize+=other.crtSize;
        other.root=nullptr;
        other.crtSize=0;
    }
};

}

#endif