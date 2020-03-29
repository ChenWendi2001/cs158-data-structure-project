#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cmath>
namespace sjtu {

template<class T>
class deque {
private:
    class node {
    public:
        T* data;
        node* next;
        node* prev;

        node(const T& x, node* ne = NULL, node* pr = NULL) : next(ne), prev(pr) {
            data = new T(x);
        }
        node(T* d=NULL){data=d;next=nullptr;prev=nullptr;}
        ~node() { if(data!=NULL) {delete data;}}
    };
    int* size_of_blocks;
    node** blocks;
    node** blocks_end;
    int num_of_blocks;
    int base_now;
    int currentLength;
    node* tail;

    node* move(node* p, int n)const { while (n-->0)p = p->next; return p; }
    void merge() {
        if (num_of_blocks < 4 * base_now) return;
        for (int i = 1; i <= num_of_blocks / 2; i++) {
            blocks_end[2 * i - 2]->next = blocks[2 * i - 1];
            blocks[2 * i - 1]->prev = blocks_end[2 * i - 2];
        }
        node** temp1 = blocks; node** temp2 = blocks_end; int* temp3 = size_of_blocks;
        base_now *= 2;
        num_of_blocks /= 2;
        blocks = new node * [base_now * 4]; blocks_end = new node * [base_now * 4];
        memset(blocks,0,sizeof(node*)*base_now * 4);
        memset(blocks_end,0,sizeof(node*)*base_now * 4);
        size_of_blocks = new int[base_now * 4];
        memset(size_of_blocks,0,sizeof(int)*base_now * 4);
        for (int i = 0; i < num_of_blocks; i++) {
            blocks_end[i] = temp2[2 * i + 1];
            blocks[i] = temp1[2 * i];
            size_of_blocks[i] = temp3[2 * i] + temp3[2 * i + 1];
        }
        delete[]temp1;
        delete[]temp2;
        delete[]temp3;
    }
    void check(int n) {
        merge();
        if (size_of_blocks[n] < 2 * base_now) return;
        node* temp1 = move(blocks[n], base_now - 1);
        node* temp2 = temp1->next;
        node* temp3 = blocks_end[n];
        temp1->next = NULL; temp2->prev = NULL; blocks_end[n] = temp1;
        int temp_size = size_of_blocks[n];
        size_of_blocks[n]  = base_now;
        for (int i = num_of_blocks; i >= n + 2; i--) {
            blocks_end[i] = blocks_end[i - 1];
            blocks[i] = blocks[i - 1];
            size_of_blocks[i] = size_of_blocks[i - 1];
        }
        size_of_blocks[n + 1]=temp_size-size_of_blocks[n];
        blocks_end[n + 1] = temp3;
        blocks[n + 1] = temp2;
        num_of_blocks++;
        merge();
    }
    bool checkEmptyblock(int n) {
        if (size_of_blocks[n] == 0) {
            for (int i = n; i <= num_of_blocks - 2; i++) {
                blocks[i] = blocks[i + 1];
                blocks_end[i] = blocks_end[i + 1];
                size_of_blocks[i] = size_of_blocks[i + 1];
            }
            blocks[num_of_blocks-1]=nullptr;blocks_end[num_of_blocks-1]=nullptr;
            size_of_blocks[num_of_blocks-1]=0;
            num_of_blocks--;
            if(currentLength) {blocks_end[num_of_blocks-1]->next=tail;tail->prev=blocks_end[num_of_blocks-1];}
            if(!num_of_blocks) {num_of_blocks++;return false;}
            return true;
        }
        else return false;
    }

public:
    class const_iterator;
    class iterator {
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
    public:
        /**
         * return a new iterator which pointer n-next elements
         *   even if there are not enough elements, the behaviour is **undefined**.
         * as well as operator-
         */
        node** map;
        node** map_end;
        int num;
        node* cur;
        int* size;
        int index;
        iterator(node** m = NULL, node** m2 = NULL, int n = 0, node* c = NULL, int* s = NULL,int i=0) :map(m), map_end(m2), num(n), cur(c), size(s),index(i) {}
        iterator operator+(const int& n) const {
            //TODO
            if(n==0) return *this;
            if(n<0) { return *this-(-n);}
            int mov = n;
            iterator newone(map, map_end, num, cur, size,index);
            while(1){
                if(newone.index+mov<newone.size[newone.num]){
                    while(mov--){newone.cur=newone.cur->next;newone.index++;}
                    return newone;
                }
                else if(newone.index+mov==newone.size[newone.num]&&newone.map_end[newone.num]->next!=NULL){
                    while(mov--){newone.cur=newone.cur->next;newone.index++;}
                    return newone;
                }
                else{
                    mov -= newone.size[newone.num] - newone.index;
                    newone.num++;
                    newone.cur = newone.map[newone.num];
                    newone.index = 0;
                }
            }
        }
        iterator operator-(const int& n) const {
            //TODO
            if(n<0) { return *this+(-n);}
            int mov = n;
            iterator newone(map, map_end, num, cur, size,index);
            for(int i = num;i>=0;i--){
                if(mov<=newone.index){
                    while(mov--){
                        newone.cur = newone.cur->prev;
                        newone.index--;
                    }
                    return newone;
                }
                else if(i==num){
                    newone.num--;mov-=newone.index+1;
                    newone.cur = newone.map_end[newone.num];
                    newone.index = newone.size[newone.num]-1;
                }
                else{
                    mov-=newone.size[newone.num];newone.num--;
                    newone.cur = newone.map_end[newone.num];
                    newone.index = newone.size[newone.num]-1;
                }
            }
        }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
        int operator-(const iterator& rhs) const {
            //TODO
            if (map != rhs.map) throw invalid_iterator();
            int sum = 0; int a = num; int b = rhs.num;
            if (a >= b) {
                while (b != a) { sum += size[b]; b++; }
                return sum - rhs.index + index;
            }
            else if (a < b) {
                while (a != b) { sum += size[a]; a++; }
                return -(sum - index + rhs.index);
            }
        }

        iterator& operator+=(const int& n) {
            if(n==0) return *this;
            if(n<0) {*this-=-n;return*this;}
            //TODO
            int mov = n;
            while(1){
                if(index+mov<size[num]){
                    while(mov--){cur=cur->next;index++;}
                    return *this;
                }
                else if(index+mov==size[num]&&map_end[num]->next!=NULL){
                    while(mov--){cur=cur->next;index++;}
                    return *this;
                }
                else{
                    mov -= size[num] - index;
                    num++;
                    cur = map[num];
                    index = 0;
                }
            }
        }

        iterator& operator-=(const int& n) {
            if(n==0) return *this;
            if(n<0) {*this+=-n;return*this;}
            //TODO
            int mov = n;
            for(int i = num;i>=0;i--){
                if(mov<=index){
                    while(mov--){
                        cur = cur->prev;
                        index--;
                    }
                    return *this;
                }
                else if(i==num){
                    num--;mov-=index+1;
                    cur = map_end[num];
                    index = size[num]-1;
                }
                else{
                    mov-=size[num];num--;
                    cur = map_end[num];
                    index = size[num]-1;
                }
            }
        }
            /**
             * TODO iter++
             */
        iterator operator++(int) {

            iterator newone(map, map_end, num, cur, size,index);
            if (cur&&cur->next) {cur = cur->next;index++;}
            else {
                num++;
                cur = map[num];
                index = 0;
            }
            return newone;
        }
            /**
             * TODO ++iter
             */
        iterator& operator++() {

            if (cur&&cur->next) {cur = cur->next;index++;}
            else {
                num++;
                cur = map[num];
                index=0;
            }
                return *this;
        }
            /**
             * TODO iter--
             */
        iterator operator--(int) {
            iterator newone(map, map_end, num, cur, size,index);
            if (cur&&cur->prev) {cur = cur->prev;index--;}
            else {
                num--;
                cur = map_end[num];
                index = size[num]-1;
            }
            return newone;
        }
            /**
             * TODO --iter
             */
        iterator& operator--() {
            if(num==0&&index==0){num--;index--;return *this;}
            if (cur&&cur->prev) {cur = cur->prev;index--;}
            else {
                num--;
                cur = map_end[num];
                index = size[num]-1;
            }
            return *this;
        }
            /**
             * TODO *it
             */
        T& operator*() const {
            if(num<0||index<0||index>=size[num]) throw invalid_iterator();
            return *(cur->data);
        }
            /**
             * TODO it->field
             */
        T* operator->() const noexcept {
            if(num<0||index<0||index>=size[num]) throw invalid_iterator();
            return cur->data;
        }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
        bool operator==(const iterator& rhs) const {
            return cur == rhs.cur;
        }
        bool operator==(const const_iterator& rhs) const {
            return cur == rhs.cur;
        }
            /**
             * some other operator for iterator.
             */
        bool operator!=(const iterator& rhs) const {
            return cur != rhs.cur;
        }
        bool operator!=(const const_iterator& rhs) const {
            return cur != rhs.cur;
        }
    };

    class const_iterator {
        // it should has similar member method as iterator.
        //  and it should be able to construct from an iterator.
        private:
            // data members.
        public:
        node** map;
        node** map_end;
        int num;
        node* cur;
        int* size;
        int index;
        const_iterator(node** m = NULL, node** m2 = NULL, int n = 0, node* c = NULL, int* s = NULL,int i=0) :map(m), map_end(m2), num(n), cur(c), size(s),index(i) {}
        const_iterator(const const_iterator& other) {
                // TODO
            map = other.map;
            map_end = other.map_end;
            num = other.num;
            cur = other.cur;
            size = other.size;
            index = other.index;
        }
        const_iterator(const iterator& other) {
                // TODO
            map = other.map;
            map_end = other.map_end;
            num = other.num;
            cur = other.cur;
            size = other.size;
            index = other.index;
        }
        const_iterator operator+(const int& n) const {
            //TODO
            if(n==0) return *this;
            if(n<0) { return *this-(-n);}
            int mov = n;
            iterator newone(map, map_end, num, cur, size,index);
            while(1){
                if(newone.index+mov<newone.size[newone.num]){
                    while(mov--){newone.cur=newone.cur->next;newone.index++;}
                    return newone;
                }
                else if(newone.index+mov==newone.size[newone.num]&&newone.map_end[newone.num]->next!=NULL){
                    while(mov--){newone.cur=newone.cur->next;newone.index++;}
                    return newone;
                }
                else{
                    mov -= newone.size[newone.num] - newone.index;
                    newone.num++;
                    newone.cur = newone.map[newone.num];
                    newone.index = 0;
                }
            }
        }
        const_iterator operator-(const int& n) const {
            //TODO
            if(n==0) return *this;
            if(n<0) { return *this+(-n);}
            int mov = n;
            iterator newone(map, map_end, num, cur, size,index);
            for(int i = num;i>=0;i--){
                if(mov<=newone.index){
                    while(mov--){
                        newone.cur = newone.cur->prev;
                        newone.index--;
                    }
                    return newone;
                }
                else if(i==num){
                    newone.num--;mov-=newone.index+1;
                    newone.cur = newone.map_end[newone.num];
                    newone.index = newone.size[newone.num]-1;
                }
                else{
                    mov-=newone.size[newone.num];newone.num--;
                    newone.cur = newone.map_end[newone.num];
                    newone.index = newone.size[newone.num]-1;
                }
            }
        }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
        int operator-(const const_iterator& rhs) const {
            //TODO

            if (map != rhs.map) throw invalid_iterator();
            int sum = 0; int a = num; int b = rhs.num;
            if (a >= b) {
                while (b != a) { sum += size[b]; b++; }
                return sum - rhs.index + index;
            }
            else if (a < b) {
                while (a != b) { sum += size[a]; a++; }
                return -(sum - index + rhs.index);
            }
        }

        const_iterator& operator+=(const int& n) {
            if(n==0) return *this;
            if(n<0) {*this-=-n;return*this;}
            //TODO
            int mov = n;
            while(1){
                if(index+mov<size[num]){
                    while(mov--){cur=cur->next;index++;}
                    return *this;
                }
                else if(index+mov==size[num]&&map_end[num]->next!=NULL){
                    while(mov--){cur=cur->next;index++;}
                    return *this;
                }
                else{
                    mov -= size[num] - index;
                    num++;
                    cur = map[num];
                    index = 0;
                }
            }
        }

        const_iterator& operator-=(const int& n) {
            if(n==0) return *this;
            if(n<0) {*this+=-n;return*this;}
            //TODO
            int mov = n;
            for(int i = num;i>=0;i--){
                if(mov<=index){
                    while(mov--){
                        cur = cur->prev;
                        index--;
                    }
                    return *this;
                }
                else if(i==num){
                    num--;mov-=index+1;
                    cur = map_end[num];
                    index = size[num]-1;
                }
                else{
                    mov-=size[num];num--;
                    cur = map_end[num];
                    index = size[num]-1;
                }
            }
        }
            /**
             * TODO iter++
             */
        const_iterator operator++(int) {
            iterator newone(map, map_end, num, cur, size,index);
            if (cur&&cur->next) {cur = cur->next;index++;}
            else {
                num++;
                cur = map[num];
                index = 0;
            }
            return newone;
        }
            /**
             * TODO ++iter
             */
        const_iterator& operator++() {

            if (cur&&cur->next) {cur = cur->next;index++;}
            else {
                num++;
                cur = map[num];
                index=0;
            }
                return *this;
        }
            /**
             * TODO iter--
             */
        const_iterator operator--(int) {
            iterator newone(map, map_end, num, cur, size,index);
            if (cur&&cur->prev) {cur = cur->prev;index--;}
            else {
                num--;
                cur = map_end[num];
                index = size[num]-1;
            }
            return newone;
        }
            /**
             * TODO --iter
             */
        const_iterator& operator--() {
            if(num==0&&index==0){num--;index--;return *this;}
            if (cur&&cur->prev) {cur = cur->prev;index--;}
            else {
                num--;
                cur = map_end[num];
                index = size[num]-1;
            }
            return *this;
        }
            /**
             * TODO *it
             */
        const T& operator*() const {
            if(num<0||index<0||index>=size[num]) throw invalid_iterator();
            return *(cur->data);
        }
            /**
             * TODO it->field
             */
        const T* operator->() const noexcept {
            if(num<0||index<0||index>=size[num]) throw invalid_iterator();
            return (cur->data);
        }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
        bool operator==(const iterator& rhs) const {
            return cur == rhs.cur;
        }
        bool operator==(const const_iterator& rhs) const {
            return cur == rhs.cur;
        }
            /**
             * some other operator for iterator.
             */
        bool operator!=(const iterator& rhs) const {
            return cur != rhs.cur;
        }
        bool operator!=(const const_iterator& rhs) const {
            return cur != rhs.cur;
        }
    };
    /**
     * TODO Constructors
     */
    deque(int base = 16) {
        base_now = sqrt(base);
        size_of_blocks = new int[base_now * 4];
        memset(size_of_blocks,0,sizeof(int)*base_now * 4);
        blocks = new node * [base_now * 4];
        memset(blocks,0,sizeof(node*)*base_now*4);
        blocks_end = new node * [base_now * 4];
        memset(blocks_end,0,sizeof(node*)*base_now*4);
        num_of_blocks = 1;
        currentLength = 0;
        tail = new node;
        memset(tail,0,sizeof(node));
    }
    deque(const deque &other) {
        tail = new node;
        memset(tail,0,sizeof(node));
        base_now = other.base_now;
        num_of_blocks = other.num_of_blocks;
        size_of_blocks = new int[4*base_now];
        memset(size_of_blocks,0,sizeof(int)*4*base_now);
        currentLength = other.currentLength;
        blocks = new node * [4*base_now];
        memset(blocks,0,sizeof(node*)*base_now*4);
        blocks_end = new node * [4*base_now];
        memset(blocks_end,0,sizeof(node*)*base_now*4);
        for (int i = 0; i < num_of_blocks; i++) {
            if(!currentLength) break;
            size_of_blocks[i] = other.size_of_blocks[i];
            node* temp2 = other.blocks[i];
            node* temp = blocks[i] = new node(*(temp2->data));
            for (int j = 1; j < size_of_blocks[i]; j++) {
                if(temp2->next == other.tail) {break;}
                temp->next = new node(*(temp2->next->data), NULL, temp);
                temp = temp->next;
                temp2 = temp2->next;
            }
            blocks_end[i] = temp;
        }
        if(currentLength)blocks_end[num_of_blocks-1]->next=tail;
        tail->prev = blocks_end[num_of_blocks-1];
    }
    /**
     * TODO Deconstructor
     */
    ~deque() {
        for (int i = 0; i < num_of_blocks; i++) {
            node* p = blocks[i];
            while (p&&p!=tail) {
                node* temp = p->next;
                delete p;
                p = temp;
            }
        }
        delete[]blocks; delete[]blocks_end;
        delete[]size_of_blocks;
        delete tail;
    }
    /**
     * TODO assignment operator
     */
    deque &operator=(const deque &other) {
        if (this == &other) return *this;
        for (int i = 0; i < num_of_blocks; i++) {
            node* p = blocks[i];
            while (p&&p!=tail) {
                node* temp = p->next;
                delete p;
                p = temp;
            }
        }
        delete[]blocks; delete[]blocks_end;
        delete[]size_of_blocks;

        base_now = other.base_now;
        num_of_blocks = other.num_of_blocks;
        size_of_blocks = new int[4*base_now];
        memset(size_of_blocks,0,sizeof(int)*4*base_now);
        currentLength = other.currentLength;
        blocks = new node * [4*base_now];
        memset(blocks,0,sizeof(node*)*base_now*4);
        blocks_end = new node * [4*base_now];
        memset(blocks_end,0,sizeof(node*)*base_now*4);
        for (int i = 0; i < num_of_blocks; i++) {
            if(!currentLength) break;
            size_of_blocks[i] = other.size_of_blocks[i];
            node* temp2 = other.blocks[i];
            node* temp = blocks[i] = new node(*(temp2->data));
            for (int j = 1; j < size_of_blocks[i]; j++) {
                if(temp2->next == other.tail) {break;}
                temp->next = new node(*(temp2->next->data), NULL, temp);
                temp = temp->next;
                temp2 = temp2->next;
            }
            blocks_end[i] = temp;
        }
        if(currentLength)blocks_end[num_of_blocks-1]->next=tail;
        tail->prev = blocks_end[num_of_blocks-1];
        return*this;
    }
    /**
     * access specified element with bounds checking
     * throw index_out_of_bound if out of bound.
     */
    T & at(const size_t &pos) {
        int posi = pos;
        if (currentLength <= posi||posi<0) throw index_out_of_bound();
        for (int i = 0; i < num_of_blocks; i++) {
            if (posi < size_of_blocks[i]) {
                node* temp = move(blocks[i], posi);
                return *(temp->data);
            }
            else posi -= size_of_blocks[i];
        }
    }
    const T & at(const size_t &pos) const {
        int posi = pos;
        if (currentLength <= posi||posi<0) throw index_out_of_bound();
        for (int i = 0; i < num_of_blocks; i++) {
            if (posi < size_of_blocks[i]) {
                node* temp = move(blocks[i], posi);
                return *(temp->data);
            }
            else posi -= size_of_blocks[i];
        }
    }
    T & operator[](const size_t &pos) {
        int posi = pos;
        if (currentLength <= posi||posi<0) throw index_out_of_bound();
        for (int i = 0; i < num_of_blocks; i++) {
            if (posi < size_of_blocks[i]) {
                node* temp = move(blocks[i], posi);
                return *(temp->data);
            }
            else posi -= size_of_blocks[i];
        }
    }
    const T & operator[](const size_t &pos) const {
        int posi = pos;
        if (currentLength <= posi||posi<0) throw index_out_of_bound();
        for (int i = 0; i < num_of_blocks; i++) {
            if (posi < size_of_blocks[i]) {
                node* temp = move(blocks[i], posi);
                return *(temp->data);
            }
            else posi -= size_of_blocks[i];
        }
    }
    /**
     * access the first element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const {
        if (currentLength == 0) throw container_is_empty();
        return *(blocks[0]->data);
    }
    /**
     * access the last element
     * throw container_is_empty when the container is empty.
     */
    const T & back() const {
        if (currentLength == 0) throw container_is_empty();
        node* p = blocks_end[num_of_blocks - 1];
        return *(p->data);
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        if(currentLength==0)return end();
        return iterator(blocks, blocks_end, 0, blocks[0], size_of_blocks,0);
    }
    const_iterator cbegin() const {
        if(currentLength==0)return cend();
        return const_iterator(blocks, blocks_end, 0, blocks[0], size_of_blocks,0);
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return iterator(blocks, blocks_end, num_of_blocks - 1, tail, size_of_blocks,size_of_blocks[num_of_blocks-1]);
    }
    const_iterator cend() const {
        return const_iterator(blocks, blocks_end, num_of_blocks - 1, tail, size_of_blocks,size_of_blocks[num_of_blocks-1]);
    }
    /**
     * checks whether the container is empty.
     */
    bool empty() const {
        return currentLength == 0;
    }
    /**
     * returns the number of elements
     */
    size_t size() const {
        return currentLength;
    }
    /**
     * clears the contents
     */
    void clear() {
        for (int i = 0; i < num_of_blocks; i++) {
            node* p = blocks[i];
            while (p&&p!=tail) {
                node* temp = p->next;
                delete p;
                p = temp;
            }
        }
        delete []size_of_blocks;delete []blocks;delete []blocks_end;
        base_now = 4;
        size_of_blocks = new int[base_now * 4];
        memset(size_of_blocks,0,sizeof(int)*base_now * 4);
        blocks = new node * [base_now * 4];
        memset(blocks,0,sizeof(node*)*base_now*4);
        blocks_end = new node * [base_now * 4];
        memset(blocks_end,0,sizeof(node*)*base_now*4);
        num_of_blocks = 1;
        currentLength = 0;
    }
    /**
     * inserts elements at the specified locat on in the container.
     * inserts value before pos
     * returns an iterator pointing to the inserted value
     *     throw if the iterator is invalid or it point to a wrong place.
     */
    iterator insert(iterator pos, const T &value) {
        if (pos.map != blocks) throw invalid_iterator();
        if (pos==end()) {
            push_back(value);
            return end()-1;
        }
        if(pos==begin()){
            push_front(value);
            return begin();
        }
        node* temp_next = pos.cur;
        node* temp_prev = pos.cur->prev;
        node*temp_cur=NULL;
        if (!temp_prev) {
            temp_cur=pos.map[pos.num] = new node(value, temp_next, temp_prev);
            if (!temp_next||temp_next==tail) pos.map_end[pos.num] = pos.map[pos.num];
            else {
                temp_next->prev = pos.map[pos.num];
            }
        }
        else {
            temp_cur=temp_prev->next = new node(value, temp_next, temp_prev);
            if (!temp_next||temp_next==tail) pos.map_end[pos.num] = temp_prev->next;
            else {
                temp_next->prev = temp_prev->next;
            }
        }
        pos.size[pos.num]++;
        currentLength++;
        //check(pos.num);
        return iterator(blocks, blocks_end, pos.num, temp_cur, size_of_blocks,pos.index);
    }
    /**
     * removes specified element at pos.
     * removes the element at pos.
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid or it points to a wrong place.
     */
    iterator erase(iterator pos) {
        if (currentLength == 0 || pos.map != blocks) throw invalid_iterator();
        if(pos.cur->next == tail){
            pop_back();
            return end();
        }
        node* temp_next = pos.cur->next;
        node* temp_prev = pos.cur->prev;
        iterator newone = pos+1;
        newone.index--;
        if (!temp_prev) {
            delete pos.cur;
            pos.map[pos.num] = temp_next;
            if (!temp_next) {pos.map_end[pos.num] = temp_prev;newone.index=0;newone.num--;}
            else {
                temp_next->prev = NULL;
            }
        }
        else {
            delete pos.cur;
            temp_prev->next = temp_next;
            if (!temp_next) {pos.map_end[pos.num] = temp_prev;newone.index=0;}
            else temp_next->prev = temp_prev;
        }
        pos.size[pos.num]--;
        currentLength--;
        checkEmptyblock(pos.num);
        return newone;
    }
    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        if (currentLength == 0) {
            blocks[0] = blocks_end[0] = new node(value,tail);
            tail->prev=blocks_end[0];
        }
        else {
            blocks_end[num_of_blocks - 1]->next = new node(value, tail, blocks_end[num_of_blocks - 1]);
            blocks_end[num_of_blocks - 1] = blocks_end[num_of_blocks - 1]->next;
            tail->prev=blocks_end[num_of_blocks - 1];
        }
        currentLength++;
        size_of_blocks[num_of_blocks - 1]++;
        check(num_of_blocks - 1);
    }
    /**
     * removes the last element
     *     throw when the container is empty.
     */
    void pop_back() {
        if (currentLength == 0) throw container_is_empty();
        node* temp = blocks_end[num_of_blocks - 1];
        blocks_end[num_of_blocks - 1] = temp->prev;
        if (temp->prev) {temp->prev->next = tail;tail->prev = temp->prev;}
        tail->prev = temp->prev;
        size_of_blocks[num_of_blocks - 1]--; currentLength--;
        delete temp;
        checkEmptyblock(num_of_blocks - 1);
    }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        if(currentLength==0){
            blocks[0] = blocks_end[0] = new node(value,tail);
            tail->prev = blocks[0];
        }
        else {
            blocks[0]->prev = new node(value, blocks[0], NULL);
            blocks[0] = blocks[0]->prev;
        }
        size_of_blocks[0]++;
        currentLength++;
        check(0);
    }
    /**
     * removes the first element.
     *     throw when the container is empty.
     */
    void pop_front() {
        if (currentLength == 0) throw container_is_empty();
        node* temp = blocks[0];
        blocks[0] = temp->next;
        if (blocks[0]) blocks[0]->prev = NULL;
        size_of_blocks[0]--; currentLength--;
        delete temp;
        checkEmptyblock(0);
    }
};

}

#endif
