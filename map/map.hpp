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

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
private:
    struct AvlNode{
        value_type *data;
        AvlNode *parent;
        AvlNode *left;
        AvlNode *right;
        size_t height;
        AvlNode():data(nullptr),parent(nullptr),left(nullptr),right(nullptr),height(0){}
        AvlNode(const value_type &x,AvlNode *p=nullptr,AvlNode *l=nullptr,AvlNode *r=nullptr,size_t h=1):parent(p),left(l),right(r),height(h){
            data=(value_type *) malloc(sizeof(value_type));
            new(data) value_type(x.first,x.second);
        }
        ~AvlNode(){
            if (data){
                data->first.~Key();
                data->second.~T();
                free(data);
            }
        }
    };

    // AvlNode *root;
    AvlNode *end_node;
    size_t length;

    inline size_t maxHeight(size_t x,size_t y) const {return x>y?x:y;}
    inline bool equal_key(Key x,Key y) const {return !(Compare()(x,y)||Compare()(y,x));}
    inline size_t height(AvlNode *rt) const {return rt?rt->height:0;}
    AvlNode *find_begin() const{
        AvlNode *p=end_node;
        while (p->left) p=p->left;
        return p;
    }
    void copy(AvlNode *&rt,AvlNode *other) const{
        if (!other) return;
        rt=new AvlNode(*(other->data));
        rt->height=other->height;
        copy(rt->left,other->left);
        if (rt->left) rt->left->parent=rt;
        copy(rt->right,other->right);
        if (rt->right) rt->right->parent=rt;
    }
    void clear(AvlNode *rt){
        if (!rt) return;
        clear(rt->left);
        clear(rt->right);
        delete rt;
    }
    AvlNode *find_node(const Key &x,AvlNode *&rt) const{
        if (!rt||equal_key(rt->data->first,x)) return rt;
        if (Compare()(x,rt->data->first))
            return find_node(x,rt->left);
        else
            return find_node(x,rt->right);
    }
    void LL(AvlNode *&rt){
        AvlNode *rt1=rt->left;
        rt1->parent=rt->parent;
        rt->left=rt1->right;
        if (rt1->right) rt1->right->parent=rt;
        rt1->right=rt;
        rt->parent=rt1;
        rt->height=maxHeight(height(rt->left),height(rt->right))+1;
        rt1->height=maxHeight(height(rt1->left),height(rt))+1;
        rt=rt1;
    }
    void RR(AvlNode *&rt){
        AvlNode *rt1=rt->right;
        rt1->parent=rt->parent;
        rt->right=rt1->left;
        if (rt1->left) rt1->left->parent=rt;
        rt1->left=rt;
        rt->parent=rt1;
        rt->height=maxHeight(height(rt->left),height(rt->right))+1;
        rt1->height=maxHeight(height(rt1->right),height(rt))+1;
        rt=rt1;
    }
    void LR(AvlNode *&rt){
        RR(rt->left);
        LL(rt);
    }
    void RL(AvlNode *&rt){
        LL(rt->right);
        RR(rt);
    }
    pair<AvlNode*, bool> insert(const value_type &value,AvlNode *&rt,AvlNode *pa){
        if (!rt) {
            rt=new AvlNode(value,pa);
            return pair<AvlNode*,bool>(rt,true);
        }else if (equal_key(value.first,rt->data->first)){
            return pair<AvlNode*,bool>(rt,false);
        }else if (Compare()(value.first,rt->data->first)){
            pair<AvlNode*,bool> p=insert(value,rt->left,rt);
            if (!p.second) return p;
            if (height(rt->left)-height(rt->right)==2){
                if (Compare()(value.first,rt->left->data->first))
                    LL(rt);
                else
                    LR(rt);
            }
            rt->height=maxHeight(height(rt->left),height(rt->right))+1;
            return p;
        }else{
            pair<AvlNode*,bool> p=insert(value,rt->right,rt);
            if (!p.second) return p;
            if (height(rt->right)-height(rt->left)==2){
                if (Compare()(rt->right->data->first,value.first))
                    RR(rt);
                else
                    RL(rt);
            }
            rt->height=maxHeight(height(rt->left),height(rt->right))+1;
            return p;
        }
    }
    bool adjust(AvlNode *&rt,int subTree){
        if (subTree){
            if (height(rt->left)==height(rt->right)){
                --rt->height;
                return false;
            }
            if (height(rt->left)-height(rt->right)==1) return true;
            if (height(rt->left->right)>height(rt->left->left)){
                LR(rt);
                return false;
            }
            LL(rt);
            if (height(rt->left)==height(rt->right))
                return false;
            else
                return true;
        }else{
            if (height(rt->right)==height(rt->left)){
                --rt->height;
                return false;
            }
            if (height(rt->right)-height(rt->left)==1) return true;
            if (height(rt->right->left)>height(rt->right->right)){
                RL(rt);
                return false;
            }
            RR(rt);
            if (height(rt->left)==height(rt->right))
                return false;
            else
                return true;
        }
    }
    bool remove(const Key &x,AvlNode *&rt){
        if (!rt) return true;
        if (equal_key(x,rt->data->first)){
            if (!rt->left||!rt->right){
                AvlNode *tmp=rt;
                rt=(rt->left)?rt->left:rt->right;
                if (rt) rt->parent=tmp->parent;
                delete tmp;
                return false;
            }else{
                AvlNode *tmp=rt->right;
                while (tmp->left) tmp=tmp->left;

                bool flag=tmp->parent->left==tmp;
                AvlNode *new_node=new AvlNode(*(tmp->data),tmp->parent,tmp->left,tmp->right,tmp->height);
                if (flag)
                    new_node->parent->left=new_node;
                else
                    new_node->parent->right=new_node;
                if (new_node->left) new_node->left->parent=new_node;
                if (new_node->right) new_node->right->parent=new_node;
                tmp->parent=rt->parent;
                tmp->left=rt->left;
                tmp->right=rt->right;
                tmp->height=rt->height;
                if (tmp->left) tmp->left->parent=tmp;
                if (tmp->right) tmp->right->parent=tmp;
                delete rt;
                rt=tmp;
                if (remove(tmp->data->first,rt->right)) return true;
                return adjust(rt,1);
            }
        }
        if (Compare()(x,rt->data->first)){
            if (remove(x,rt->left)) return true;
            return adjust(rt,0);
        }else{
            if (remove(x,rt->right)) return true;
            return adjust(rt,1);
        }
    }

public:
	class const_iterator;
	class iterator {
        friend class map;
	private:
        map *map_ptr;
        AvlNode *cur;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		iterator() {
			// TODO
            map_ptr=nullptr;
            cur=nullptr;
		}
		iterator(const iterator &other) {
			// TODO
            map_ptr=other.map_ptr;
            cur=other.cur;
		}
        iterator(map *_map_ptr,AvlNode *_cur){
            map_ptr=_map_ptr;
            cur=_cur;
        }
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
            if (cur==map_ptr->end_node) throw invalid_iterator();
            AvlNode *tmp=cur;
            if (cur->right){
                cur=cur->right;
                while (cur->left) cur=cur->left;
            }else{
                while (cur->parent->left!=cur) cur=cur->parent;
                cur=cur->parent;
            }
            return iterator(map_ptr,tmp);
        }
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {
            if (cur==map_ptr->end_node) throw invalid_iterator();
            if (cur->right){
                cur=cur->right;
                while (cur->left) cur=cur->left;
            }else{
                while (cur->parent->left!=cur) cur=cur->parent;
                cur=cur->parent;
            }
            return *this;
        }
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
            if (cur==map_ptr->find_begin()) throw invalid_iterator();
            AvlNode *tmp=cur;
            if (cur->left){
                cur=cur->left;
                while (cur->right) cur=cur->right;
            }else{
                while (cur->parent&&cur->parent->right!=cur) cur=cur->parent;
                cur=cur->parent;
            }
            return iterator(map_ptr,tmp);
        }
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
            if (cur==map_ptr->find_begin()) throw invalid_iterator();
            if (cur->left){
                cur=cur->left;
                while (cur->right) cur=cur->right;
            }else{
                while (cur->parent&&cur->parent->right!=cur) cur=cur->parent;
                cur=cur->parent;
            }
            return *this;
        }
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {
            if (cur==map_ptr->end_node) throw invalid_iterator();
            return *(cur->data);
        }
		bool operator==(const iterator &rhs) const {
            return map_ptr==rhs.map_ptr&&cur==rhs.cur;
        }
		bool operator==(const const_iterator &rhs) const {
            return map_ptr==rhs.map_ptr&&cur==rhs.cur;
        }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
            return map_ptr!=rhs.map_ptr||cur!=rhs.cur;
        }
		bool operator!=(const const_iterator &rhs) const {
            return map_ptr!=rhs.map_ptr||cur!=rhs.cur;
        }

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
            return cur->data;
        }
	};
	class const_iterator {
		// it should have similar member method as iterator.
		//  and it should be able to construct from an iterator.
    friend class map;
    private:
        const map *map_ptr;
        AvlNode *cur;
        // data members.
    public:
        const_iterator() {
            // TODO
            map_ptr=nullptr;
            cur=nullptr;
        }
        const_iterator(const const_iterator &other) {
            // TODO
            map_ptr=other.map_ptr;
            cur=other.cur;
        }
        const_iterator(const iterator &other) {
            // TODO
            map_ptr=other.map_ptr;
            cur=other.cur;
        }
        const_iterator(const map *_map_ptr,AvlNode *_cur){
            map_ptr=_map_ptr;
            cur=_cur;
        }
        // And other methods in iterator.
        // And other methods in iterator.
        // And other methods in iterator.
        /**
		 * TODO iter++
		 */
        const_iterator operator++(int) {
            if (cur==map_ptr->end_node) throw invalid_iterator();
            AvlNode *tmp=cur;
            if (cur->right){
                cur=cur->right;
                while (cur->left) cur=cur->left;
            }else{
                while (cur->parent->left!=cur) cur=cur->parent;
                cur=cur->parent;
            }
            return const_iterator(map_ptr,tmp);
        }
        /**
         * TODO ++iter
         */
        const_iterator & operator++() {
            if (cur==map_ptr->end_node) throw invalid_iterator();
            if (cur->right){
                cur=cur->right;
                while (cur->left) cur=cur->left;
            }else{
                while (cur->parent->left!=cur) cur=cur->parent;
                cur=cur->parent;
            }
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            if (cur==map_ptr->find_begin()) throw invalid_iterator();
            AvlNode *tmp=cur;
            if (cur->left){
                cur=cur->left;
                while (cur->right) cur=cur->right;
            }else{
                while (cur->parent&&cur->parent->right!=cur) cur=cur->parent;
                cur=cur->parent;
            }
            return const_iterator(map_ptr,tmp);
        }
        /**
         * TODO --iter
         */
        const_iterator & operator--() {
            if (cur==map_ptr->find_begin()) throw invalid_iterator();
            if (cur->left){
                cur=cur->left;
                while (cur->right) cur=cur->right;
            }else{
                while (cur->parent&&cur->parent->right!=cur) cur=cur->parent;
                cur=cur->parent;
            }
            return *this;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        const value_type & operator*() const {
            if (cur==map_ptr->end_node) throw invalid_iterator();
            return *(cur->data);
        }
        bool operator==(const iterator &rhs) const {
            return map_ptr==rhs.map_ptr&&cur==rhs.cur;
        }
        bool operator==(const const_iterator &rhs) const {
            return map_ptr==rhs.map_ptr&&cur==rhs.cur;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return map_ptr!=rhs.map_ptr||cur!=rhs.cur;
        }
        bool operator!=(const const_iterator &rhs) const {
            return map_ptr!=rhs.map_ptr||cur!=rhs.cur;
        }

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
         */
        const value_type* operator->() const noexcept {
            return cur->data;
        }
	};
	/**
	 * TODO two constructors
	 */
	map() {
        end_node=new AvlNode;
        length=0;
    }
	map(const map &other) {
        end_node=new AvlNode;
        copy(end_node->left,other.end_node->left);
        if (end_node->left) end_node->left->parent=end_node;
        length=other.length;
    }
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
        if (this==&other) return *this;
        clear(end_node->left);
        copy(end_node->left,other.end_node->left);
        if (end_node->left) end_node->left->parent=end_node;
        length=other.length;
        return *this;
    }
	/**
	 * TODO Destructors
	 */
	~map() {
        clear(end_node->left);
        delete end_node;
    }
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
        AvlNode *p=find_node(key,end_node->left);
        if (!p) throw index_out_of_bound();
        return p->data->second;
    }
	const T & at(const Key &key) const {
        AvlNode *p=find_node(key,end_node->left);
        if (!p) throw index_out_of_bound();
        return p->data->second;
    }
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
        AvlNode *p=find_node(key,end_node->left);
        if (p) return p->data->second;
        pair<AvlNode*,bool> tmp=insert(value_type(key,T()),end_node->left,end_node);
        ++length;
        return tmp.first->data->second;
    }
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
        AvlNode *p=find_node(key,end_node->left);
        if (!p) throw index_out_of_bound();
        return p->data->second;
    }
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
        return iterator(this,find_begin());
    }
	const_iterator cbegin() const {
        return const_iterator(this,find_begin());
    }
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
        return iterator(this,end_node);
    }
	const_iterator cend() const {
        return const_iterator(this,end_node);
    }
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
        return length==0;
    }
	/**
	 * returns the number of elements.
	 */
	size_t size() const {
        return length;
    }
	/**
	 * clears the contents
	 */
	void clear() {
        clear(end_node->left);
        end_node->left=nullptr;
        length=0;
    }
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
        pair<AvlNode*,bool> p=insert(value,end_node->left,end_node);
        if (p.second) ++length;
        return pair<iterator, bool>(iterator(this,p.first),p.second);
    }
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
        if (pos.map_ptr!=this||pos.cur==end_node) throw invalid_iterator();
        length-=count(pos->first);
        remove(pos->first,end_node->left);
    }
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
        if (find_node(key,end_node->left))
            return 1;
        else
            return 0;
    }
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
        AvlNode *p=find_node(key,end_node->left);
        if (p)
            return iterator(this,p);
        else
            return iterator(this,end_node);
    }
	const_iterator find(const Key &key) const {
        AvlNode *p=find_node(key,end_node->left);
        if (p)
            return const_iterator(this,p);
        else
            return const_iterator(this,end_node);
    }
};

}

#endif
