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
    > class linked_hashmap {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::linked_hashmap as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
    private:
        struct node{
            value_type *data;
            node *after,*pre,*next;
            node():data(nullptr),after(nullptr),pre(nullptr),next(nullptr){}
            node(const value_type &x,node *a=nullptr,node *p=nullptr,node *n=nullptr):after(a),pre(p),next(n){
                data=(value_type *) malloc(sizeof(value_type));
                new(data) value_type(x.first,x.second);
            }
            ~node(){
                if (data) {
                    data->first.~Key();
                    data->second.~T();
                    free(data);
                }
            }
        };

        class bucketList{
        public:
            node *_head;
            bucketList(): _head(nullptr){}
            ~bucketList()=default;
            void insert(node *x){
                x->after=_head;
                _head=x;
            }
            node *erase(const Key &key){
                if (!_head) return nullptr;
                node *p=_head;
                if (Equal()(_head->data->first, key)){
                    _head=p->after;
                    return p;
                }
                while (p->after&&!Equal()(p->after->data->first,key)){
                    p=p->after;
                }
                if (p->after){
                    node *tmp=p->after;
                    p->after=tmp->after;
                    return tmp;
                }
                return nullptr;
            }
            node *find(const Key &key) const{
                node *p=_head;
                while (p&&!Equal()(p->data->first,key)){
                    p=p->after;
                }
                return p;
            }
        };

        bucketList *storage;
        size_t capacity;
        static constexpr float LOAD_FACTOR=0.75;
        node *head,*tail;
        size_t length;
        static constexpr size_t mod[5]={1009,10007,100003,1000003,10000019};
        int crt;

        void resize(){
            if (crt==4) return;
            capacity=mod[++crt];
            delete[] storage;
            storage=new bucketList[capacity];
            for (node *p=head->next;p!=tail;p=p->next){
                storage[Hash()(p->data->first)%capacity].insert(p);
            }
        }

    public:
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = linked_hashmap.begin(); --it;
         *       or it = linked_hashmap.end(); ++end();
         */
        class const_iterator;
        class iterator {
            friend class linked_hashmap;
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            linked_hashmap *ptr;
            node *pos;
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
            using value_type = typename linked_hashmap::value_type;
            using pointer = value_type*;
            using reference = value_type&;
            using iterator_category = std::output_iterator_tag;


            iterator(linked_hashmap *_ptr=nullptr,node *_pos=nullptr):ptr(_ptr),pos(_pos) {
                // TODO
            }
            iterator(const iterator &other):ptr(other.ptr),pos(other.pos) {
                // TODO
            }
            /**
             * TODO iter++
             */
            iterator operator++(int) {
                if (pos==ptr->tail) throw invalid_iterator();
                node *p=pos;
                pos=pos->next;
                return iterator(ptr,p);
            }
            /**
             * TODO ++iter
             */
            iterator & operator++() {
                if (pos==ptr->tail) throw invalid_iterator();
                pos=pos->next;
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int) {
                if (pos==ptr->head->next) throw invalid_iterator();
                node *p=pos;
                pos=pos->pre;
                return iterator(ptr,p);
            }
            /**
             * TODO --iter
             */
            iterator & operator--() {
                if (pos==ptr->head->next) throw invalid_iterator();
                pos=pos->pre;
                return *this;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type & operator*() const {
                if (!pos->data) throw invalid_iterator();
                return *(pos->data);
            }
            bool operator==(const iterator &rhs) const {
                return ptr==rhs.ptr&&pos==rhs.pos;
            }
            bool operator==(const const_iterator &rhs) const {
                return ptr==rhs.ptr&&pos==rhs.pos;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return ptr!=rhs.ptr||pos!=rhs.pos;
            }
            bool operator!=(const const_iterator &rhs) const {
                return ptr!=rhs.ptr||pos!=rhs.pos;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type* operator->() const noexcept {
                return pos->data;
            }
        };

        class const_iterator {
            // it should have similar member method as iterator.
            //  and it should be able to construct from an iterator.
            friend class linked_hashmap;
        private:
            // data members.
            const linked_hashmap *ptr;
            node *pos;
        public:
            const_iterator(const linked_hashmap *_ptr=nullptr,node *_pos=nullptr):ptr(_ptr),pos(_pos) {
                // TODO
            }
            const_iterator(const const_iterator &other):ptr(other.ptr),pos(other.pos) {
                // TODO
            }
            const_iterator(const iterator &other):ptr(other.ptr),pos(other.pos) {
                // TODO
            }
            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                if (pos==ptr->tail) throw invalid_iterator();
                node *p=pos;
                pos=pos->next;
                return const_iterator(ptr,p);
            }
            /**
             * TODO ++iter
             */
            const_iterator & operator++() {
                if (pos==ptr->tail) throw invalid_iterator();
                pos=pos->next;
                return *this;
            }
            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                if (pos==ptr->head->next) throw invalid_iterator();
                node *p=pos;
                pos=pos->pre;
                return const_iterator(ptr,p);
            }
            /**
             * TODO --iter
             */
            const_iterator & operator--() {
                if (pos==ptr->head->next) throw invalid_iterator();
                pos=pos->pre;
                return *this;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            const value_type & operator*() const {
                if (!pos->data) throw invalid_iterator();
                return *(pos->data);
            }
            bool operator==(const iterator &rhs) const {
                return ptr==rhs.ptr&&pos==rhs.pos;
            }
            bool operator==(const const_iterator &rhs) const {
                return ptr==rhs.ptr&&pos==rhs.pos;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return ptr!=rhs.ptr||pos!=rhs.pos;
            }
            bool operator!=(const const_iterator &rhs) const {
                return ptr!=rhs.ptr||pos!=rhs.pos;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            const value_type* operator->() const noexcept {
                return pos->data;
            }
        };

        /**
         * TODO two constructors
         */
        linked_hashmap() {
            head=new node;
            tail=new node;
            head->next=tail;
            tail->pre=head;
            length=0;
            crt=0;
            capacity=mod[crt];
            storage=new bucketList[capacity];
        }
        linked_hashmap(const linked_hashmap &other) {
            crt=other.crt;
            capacity=mod[crt];
            storage=new bucketList[capacity];
            head=new node;
            tail=new node;
            node *p=other.head->next,*q=head;
            while (p!=other.tail){
                size_t hash=Hash()(p->data->first)%capacity;
                q->next=new node(*(p->data),storage[hash]._head,q);
                storage[hash]._head=q=q->next;
                p=p->next;
            }
            q->next=tail;
            tail->pre=q;
            length=other.length;
        }

        /**
         * TODO assignment operator
         */
        linked_hashmap & operator=(const linked_hashmap &other) {
            if (this==&other) return *this;
            delete[] storage;
            node *p=head->next,*q;
            while (p!=tail){
                q=p;
                p=p->next;
                delete q;
            }
            crt=other.crt;
            capacity=mod[crt];
            storage=new bucketList[capacity];
            p=other.head->next,q=head;
            while (p!=other.tail){
                size_t hash=Hash()(p->data->first)%capacity;
                q->next=new node(*(p->data),storage[hash]._head,q);
                storage[hash]._head=q=q->next;
                p=p->next;
            }
            q->next=tail;
            tail->pre=q;
            length=other.length;
            return *this;
        }

        /**
         * TODO Destructors
         */
        ~linked_hashmap() {
            delete[] storage;
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
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T & at(const Key &key) {
            node *p=storage[Hash()(key)%capacity].find(key);
            if (p){
                return p->data->second;
            }else{
                throw index_out_of_bound();
            }
        }
        const T & at(const Key &key) const {
            node *p=storage[Hash()(key)%capacity].find(key);
            if (p){
                return p->data->second;
            }else{
                throw index_out_of_bound();
            }
        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T & operator[](const Key &key) {
            node *p=storage[Hash()(key)%capacity].find(key);
            if (p){
                return p->data->second;
            }else{
                return insert(value_type(key,T())).first->second;
            }
        }
        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T & operator[](const Key &key) const {
            node *p=storage[Hash()(key)%capacity].find(key);
            if (p){
                return p->data->second;
            }else{
                throw index_out_of_bound();
            }
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator(this,head->next);
        }
        const_iterator cbegin() const {
            return const_iterator(this,head->next);
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(this,tail);
        }
        const_iterator cend() const {
            return const_iterator(this,tail);
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
            delete[] storage;
            node *p=head->next,*q;
            while (p!=tail){
                q=p;
                p=p->next;
                delete q;
            }
            head->next=tail;
            tail->pre=head;
            length=0;
            crt=0;
            capacity=mod[crt];
            storage=new bucketList[capacity];
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            size_t hash=Hash()(value.first)%capacity;
            if (node *p=storage[hash].find(value.first)){
                return pair<iterator,bool>(iterator(this,p),false);
            }
            if (length>capacity*LOAD_FACTOR) resize();
            hash=Hash()(value.first)%capacity;
            node *p=new node(value, storage[hash]._head, tail->pre, tail);
            tail->pre->next=p;
            tail->pre=p;
            storage[hash]._head=p;
            length++;
            return pair<iterator,bool>(iterator(this,p),true);
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos.ptr!=this||pos==end()) throw invalid_iterator();
            pos.pos->pre->next=pos.pos->next;
            pos.pos->next->pre=pos.pos->pre;
            delete storage[Hash()(pos->first)%capacity].erase(pos->first);
            length--;
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         */
        size_t count(const Key &key) const {
            node *p=storage[Hash()(key)%capacity].find(key);
            if (p){
                return 1;
            }else{
                return 0;
            }
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            node *p=storage[Hash()(key)%capacity].find(key);
            if (p){
                return iterator(this,p);
            }else{
                return end();
            }
        }
        const_iterator find(const Key &key) const {
            node *p=storage[Hash()(key)%capacity].find(key);
            if (p){
                return const_iterator(this,p);
            }else{
                return cend();
            }
        }
    };

}

#endif