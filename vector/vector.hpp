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
    T** data;
    size_t currentLength;
    size_t maxSize;
    void doubleSpace(){
        T** temp = data;
        int temp_size = maxSize;
        maxSize *= 2;
        data = new T*[maxSize];
        for(int i = 0;i<currentLength;i++){
            data[i]=new T(*temp[i]);
        }
        for(int i = 0;i<temp_size;i++) delete temp[i];
        delete []temp;
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
        T** ptr;
        T** start;
    public:
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator(){};
        iterator(T** a,T** b):ptr(a),start(b){}
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
            if(start != rhs.start) throw invalid_iterator();
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
            int temp = *this;
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
            int temp = *this;
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
            return **this->ptr;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator==(const iterator &rhs) const {
            if(start!=rhs.start) throw invalid_iterator();
            return ptr==rhs.ptr;
        }
        bool operator==(const const_iterator &rhs) const {
            if(start!=rhs.start) throw invalid_iterator();
            return ptr==rhs.ptr;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            if(start!=rhs.start) throw invalid_iterator();
            return ptr!=rhs.ptr;
        }
        bool operator!=(const const_iterator &rhs) const {
            if(start!=rhs.start) throw invalid_iterator();
            return ptr!=rhs.ptr;
        }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
    private:

        T** ptr;
        T** start;
    public:
        const_iterator(){};
        const_iterator(T** a,T** b):ptr(a),start(b){}
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
            if(start != rhs.start) throw invalid_iterator();
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
            int temp = *this;
            ptr+=1;
            return temp;
        }

        const_iterator& operator++() {
            ptr+=1;
            return *this;
        }

        const_iterator operator--(int) {
            int temp = *this;
            ptr-=1;
            return temp;
        }

        const_iterator& operator--() {
            ptr-=1;
            return *this;
        }

        const T& operator*() const{
            return **this->ptr;
        }

        bool operator==(const iterator &rhs) const {
            if(start!=rhs.start) throw invalid_iterator();
            return ptr==rhs.ptr;
        }
        bool operator==(const const_iterator &rhs) const {
            if(start!=rhs.start) throw invalid_iterator();
            return ptr==rhs.ptr;
        }

        bool operator!=(const iterator &rhs) const {
            if(start!=rhs.start) throw invalid_iterator();
            return ptr!=rhs.ptr;
        }
        bool operator!=(const const_iterator &rhs) const {
            if(start!=rhs.start) throw invalid_iterator();
            return ptr!=rhs.ptr;
        }
    };
    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    vector(int initSize=10) {
        data = new T*[initSize];
        maxSize = initSize;
        currentLength = 0;
    }
    vector(const vector &other) {
        maxSize = other.maxSize;
        currentLength = other.currentLength;
        data = new T*[currentLength];
        for(int i = 0 ;i<currentLength;i++){
            data[i] = new T(*(other.data[i]));
        }
    }
    /**
     * TODO Destructor
     */
    ~vector() {
        for(int i = 0;i<currentLength;i++) delete data[i];
        delete []data;
    }
    /**
     * TODO Assignment operator
     */
    vector &operator=(const vector &other) {
        if (this == &other) return *this;
        for(int i = 0;i<currentLength;i++) delete data[i];
        delete []data;
        maxSize = other.maxSize;
        currentLength = other.currentLength;
        data = new T*[currentLength];
        for(int i = 0 ;i<currentLength;i++){
            data[i] = new T(*(other.data[i]));
        }
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T & at(const size_t &pos) {
        if(pos>=currentLength) throw index_out_of_bound();
        return *data[pos];
    }

    const T & at(const size_t &pos) const {
        if(pos>=currentLength) throw index_out_of_bound();
        return *data[pos];
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T & operator[](const size_t &pos) {
        if(pos>=currentLength) throw index_out_of_bound();
        return *data[pos];
    }
    const T & operator[](const size_t &pos) const {
        if(pos>=currentLength) throw index_out_of_bound();
        return *data[pos];
    }
    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T & front() const {
        if(currentLength==0) throw container_is_empty();
        return *data[0];
    }
    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T & back() const {
        if(currentLength==0) throw container_is_empty();
        return *data[currentLength-1];
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        return vector<T>::iterator(data,data);
    }
    const_iterator cbegin() const {
        return vector<T>::const_iterator(data,data);
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return vector<T>::iterator(data+currentLength,data);
    }
    const_iterator cend() const {
        return vector<T>::const_iterator(data+currentLength,data);
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
        for(int i = 0;i<currentLength;i++) delete data[i];
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
        data[dist]=new T(value);
        return iterator(data+dist,data);
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value) {
        if(ind>maxSize) throw index_out_of_bound();
        int dist = ind;
        if(currentLength==maxSize) doubleSpace();
        currentLength++;
        for(int i = currentLength-1;i>dist;i--){
            data[i]=data[i-1];
        }
        data[dist]=new T(value);
        return iterator(data+dist,data);
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos) {
        int dist = pos-begin();
        delete data[dist];
        for(int i = dist;i<currentLength-1;i++){
            data[i]=data[i+1];
        }
        currentLength--;
        return begin()+dist;
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
        if(ind>=maxSize) throw index_out_of_bound();
        int dist = ind;
        delete data[dist];
        for(int i = dist;i<currentLength-1;i++){
            data[i]=data[i+1];
        }
        currentLength--;
        return begin()+dist;
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
        if(currentLength==maxSize) doubleSpace();
        data[currentLength++] = new T(value);
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
        if(size()==0) throw container_is_empty();
        delete data[--currentLength];
    }
};

}

#endif
