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
    T* data;
    size_t currentLength;
    size_t maxSize;
    int *flag;
    void doubleSpace(){
        T* temp = data;
        int temp_size = maxSize;
        maxSize *= 2;
        data = (T*)malloc(sizeof(T)*maxSize);
        memset(data,0,sizeof(T)*maxSize);
        for(int i = 0;i<currentLength;i++){
            data[i]=temp[i];
        }
        for(int i = 0;i<temp_size;i++) temp[i].~T();
        free(temp);
    };
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
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        T* ptr;
        int* flag;
    public:
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator(){};
        iterator(T* a,int* b):ptr(a),flag(b){}
        iterator operator+(const int &n) const {
            //TODO
            iterator newone;
            newone = *this;
            newone.ptr= ptr+n;
            return newone;
        }
        iterator operator-(const int &n) const {
            //TODO
            iterator newone;
            newone = *this;
            newone.ptr= ptr-n;
            return newone;
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
        int operator-(const iterator &rhs) const {
            if(flag != rhs.flag) throw invalid_iterator();
            return (ptr-rhs.ptr);
            //TODO
        }
        iterator& operator+=(const int &n) {
            ptr+=n;
            return *this;
            //TODO
        }
        iterator& operator-=(const int &n) {
            ptr-=n;
            return *this;
            //TODO
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            iterator temp = *this;
            ptr+=1;
            return temp;
        }
        /**
         * TODO ++iter
         */
        iterator& operator++() {
            ptr+=1;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            iterator temp = *this;
            ptr-=1;
            return temp;
        }
        /**
         * TODO --iter
         */
        iterator& operator--() {
            ptr-=1;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const{
            return *(this->ptr);
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator==(const iterator &rhs) const {
            if(flag!=rhs.flag) throw invalid_iterator();
            return ptr==rhs.ptr;
        }
        bool operator==(const const_iterator &rhs) const {
            if(flag!=rhs.flag) throw invalid_iterator();
            return ptr==rhs.ptr;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            if(flag!=rhs.flag) throw invalid_iterator();
            return ptr!=rhs.ptr;
        }
        bool operator!=(const const_iterator &rhs) const {
            if(flag!=rhs.flag) throw invalid_iterator();
            return ptr!=rhs.ptr;
        }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
    private:

        T* ptr;
        int* flag;
    public:
        const_iterator(){};
        const_iterator(T* a,int* b):ptr(a),flag(b){}
        const_iterator operator+(const int &n) const {

            const_iterator newone;
            newone = *this;
            newone.ptr= ptr+n;
            return newone;
        }
        const_iterator operator-(const int &n) const {

            const_iterator newone;
            newone = *this;
            newone.ptr= ptr-n;
            return newone;
        }

        int operator-(const const_iterator &rhs) const {
            if(flag != rhs.flag) throw invalid_iterator();
            return (ptr-rhs.ptr);

        }
        const_iterator& operator+=(const int &n) {
            ptr+=n;
            return *this;

        }
        const_iterator& operator-=(const int &n) {
            ptr-=n;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator temp = *this;
            ptr+=1;
            return temp;
        }

        const_iterator& operator++() {
            ptr+=1;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator temp = *this;
            ptr-=1;
            return temp;
        }

        const_iterator& operator--() {
            ptr-=1;
            return *this;
        }

        const T& operator*() const{
            return *(this->ptr);
        }

        bool operator==(const iterator &rhs) const {
            if(flag!=rhs.flag) throw invalid_iterator();
            return ptr==rhs.ptr;
        }
        bool operator==(const const_iterator &rhs) const {
            if(flag!=rhs.flag) throw invalid_iterator();
            return ptr==rhs.ptr;
        }

        bool operator!=(const iterator &rhs) const {
            if(flag!=rhs.flag) throw invalid_iterator();
            return ptr!=rhs.ptr;
        }
        bool operator!=(const const_iterator &rhs) const {
            if(flag!=rhs.flag) throw invalid_iterator();
            return ptr!=rhs.ptr;
        }
    };
    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    vector(int initSize=10) {
        data = (T*)malloc(sizeof(T)*initSize);
        memset(data,0,sizeof(T)*initSize);
        flag = new int;
        maxSize = initSize;
        currentLength = 0;
    }
    vector(const vector &other) {
        maxSize = other.maxSize;
        currentLength = other.currentLength;
        data = (T*)malloc(sizeof(T)*currentLength);
        memset(data,0,sizeof(T)*currentLength);
        flag = new int;
        for(int i = 0 ;i<currentLength;i++){
            data[i] = other.data[i];
        }
    }
    /**
     * TODO Destructor
     */
    ~vector() {
        for(int i = 0;i<currentLength;i++) data[i].~T();
        free(data);
        delete flag;
    }
    /**
     * TODO Assignment operator
     */
    vector &operator=(const vector &other) {
        if (this == &other) return *this;
        for(int i = 0;i<currentLength;i++) data[i].~T();
        free(data);
        maxSize = other.maxSize;
        currentLength = other.currentLength;
        data = (T*)malloc(sizeof(T)*currentLength);
        memset(data,0,sizeof(T)*currentLength);
        for(int i = 0 ;i<currentLength;i++){
            data[i] = other.data[i];
        }
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T & at(const size_t &pos) {
        if(pos>=currentLength) throw index_out_of_bound();
        return data[pos];
    }

    const T & at(const size_t &pos) const {
        if(pos>=currentLength) throw index_out_of_bound();
        return data[pos];
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T & operator[](const size_t &pos) {
        if(pos>=currentLength) throw index_out_of_bound();
        return data[pos];
    }
    const T & operator[](const size_t &pos) const {
        if(pos>=currentLength) throw index_out_of_bound();
        return data[pos];
    }
    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T & front() const {
        if(currentLength==0) throw container_is_empty();
        return data[0];
    }
    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T & back() const {
        if(currentLength==0) throw container_is_empty();
        return data[currentLength-1];
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        return vector<T>::iterator(data,flag);
    }
    const_iterator cbegin() const {
        return vector<T>::const_iterator(data,flag);
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return vector<T>::iterator(data+currentLength,flag);
    }
    const_iterator cend() const {
        return vector<T>::const_iterator(data+currentLength,flag);
    }
    /**
     * checks whether the container is empty
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
        for(int i = 0;i<currentLength;i++) data[i].~T();
        currentLength = 0;
    }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value) {
        int dist = pos-begin();
        if(currentLength==maxSize) doubleSpace();
        currentLength++;
        for(int i = currentLength-1;i>dist;i--){
            data[i]=data[i-1];
        }
        data[dist].~T();
        data[dist]=value;
        return iterator(data+dist,flag);
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value) {
        if(ind>currentLength) throw index_out_of_bound();
        int dist = ind;
        if(currentLength==maxSize) doubleSpace();
        currentLength++;
        for(int i = currentLength-1;i>dist;i--){
            data[i]=data[i-1];
        }
        if(dist!=currentLength-1)data[dist].~T();
        data[dist]=value;
        return iterator(data+dist,flag);
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos) {
        int dist = pos-begin();
        data[dist].~T();
        for(int i = dist;i<currentLength-1;i++){
            data[i]=data[i+1];
        }
        currentLength--;
        data[currentLength].~T();
        return begin()+dist;
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
        if(ind>=currentLength) throw index_out_of_bound();
        int dist = ind;
        data[dist].~T();
        for(int i = dist;i<currentLength-1;i++){
            data[i]=data[i+1];
        }
        currentLength--;
        data[currentLength].~T();
        return begin()+dist;
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
        if(currentLength==maxSize) doubleSpace();
        data[currentLength++] = value;
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
        if(size()==0) throw container_is_empty();
        data[--currentLength].~T();
    }
};

}

#endif
