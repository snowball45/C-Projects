#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm> // std::random_access_iterator_tag
#include <cstddef> // size_t
#include <stdexcept> // std::out_of_range
#include <type_traits> // std::is_same

template <class T>
class Vector {
public:
    class iterator;
private:
    T* array;
    size_t _capacity, _size;

    // You may want to write a function that grows the vector
    void grow(size_t capacity) {
        _capacity=capacity;
        T *newArray = new T[_capacity];
        std::move(array,array+_size,newArray);
        delete[] array;
        array = newArray;
    }

public:
    Vector() noexcept : array(nullptr), _capacity(0), _size(0){}
    Vector(size_t count, const T& value) : _capacity(count), _size(count) {
        array = new T[_capacity];
        for (size_t i=0;i<count;i++){array[i]=value;}
    }
    explicit Vector(size_t count) : _capacity(count), _size(count){
        array = new T[_capacity];
        for (size_t i=0;i<count;i++){array[i]=T();}
    }
    Vector(const Vector& other) : array(nullptr), _capacity(0), _size(0) {*this=other;}
    Vector(Vector&& other) noexcept : array(other.array), _capacity(other._capacity), _size(other._size){
        other.array = nullptr;
        other._size=0;
        other._capacity=0;
    }

    ~Vector() {delete[] array;}

    Vector& operator=(const Vector& other) {
        if (this != &other){
            delete[] array;
            _capacity = other._capacity;
            _size = other._size;
            array = new T[_capacity];
            for (size_t i=0;i<_size;i++){
                array[i] = other.array[i];
            }
        }
        return *this;
    }
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other){
            delete[] array;
            array = other.array;
            _capacity = other._capacity;
            _size = other._size;
            other.array = nullptr;
            other._size=0;
            other._capacity=0;
        }
        return *this;
    }

    iterator begin() noexcept {
        T *ptr = &(array[0]);
        iterator first = iterator(ptr);
        return first;
    }
    iterator end() noexcept {
        T *ptr = &(array[_size]);
        iterator last = iterator(ptr);
        return last;
    }

    [[nodiscard]] bool empty() const noexcept {if (_size==0) return true; else return false;}
    size_t size() const noexcept {return _size;}
    size_t capacity() const noexcept {return _capacity;}

    T& at(size_t pos) {
        if ((pos<0)||(pos>=_size)){throw std::out_of_range("error");}
        else{return array[pos];}
    }
    const T& at(size_t pos) const {
        if ((pos<0)||(pos>=_size)){throw std::out_of_range("error");}
        else{return array[pos];}
    }
    T& operator[](size_t pos) {return array[pos];}
    const T& operator[](size_t pos) const {return array[pos];}
    T& front() {return array[0];}
    const T& front() const {return array[0];}
    T& back() {return array[_size-1];}
    const T& back() const {return array[_size-1];}

    void push_back(const T& value) {
        if (_capacity==0){grow(1);}
        else if (_capacity==_size){grow(_capacity*2);}
        array[_size] = value;
        _size++;
    }
    void push_back(T&& value) {
        if (_capacity==0){grow(1);}
        else if (_capacity==_size){grow(_capacity*2);}
        array[_size] = std::move(value);
        _size++;
    }
    void pop_back() {array[_size]=T();_size--;}

    iterator insert(iterator pos, const T& value) {
        size_t index = pos - begin();
        if ((_size+1)>=_capacity){grow(_capacity*2);}
        T* ptr = &(array[index]);
        iterator it(ptr);
        pos = it;
        for(size_t i=_size;i>index;i--){
            array[i] = std::move(array[i-1]);
        }
        array[index] = T(value);
        ++_size;
        return pos;
    }
    iterator insert(iterator pos, T&& value) {
        size_t index = pos - begin();
        if ((_size+1)>=_capacity){grow(_capacity*2);}
        T* ptr = &(array[index]);
        iterator it(ptr);
        pos = it;
        for(size_t i=_size;i>index;i--){
            array[i] = std::move(array[i-1]);
        }
        array[index] = std::move(value);
        ++_size;
        return pos;
    }
    iterator insert(iterator pos, size_t count, const T& value) {
        size_t index = pos - begin();
        while (_size+count>=_capacity){grow(_capacity*2);}
        T* ptr = &(array[index]);
        iterator it(ptr);
        pos = it;
        for (size_t i=_size+count-1;i>=index+count;i--){
            array[i] = std::move(array[i-count]);
        }
        for (size_t i=index;i<index+count;i++){
            array[i] = T(value);
        }
        _size+=count;
        return pos;
    }
    iterator erase(iterator pos) {
        std::move(pos+1,end(),pos);
        _size--;
        return pos;
    }
    iterator erase(iterator first, iterator last) {
        std::move(last,end(),first);
        _size-=(last-first);
        return first;
    }

    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
    private:
        // Points to some element in the vector (or nullptr)
        T* _ptr;

    public:
        iterator() : _ptr(nullptr) {}
        explicit iterator(T* ptr) : _ptr(ptr){}

        // This assignment operator is done for you, please do not add more
        iterator& operator=(const iterator&) noexcept = default;

        [[nodiscard]] reference operator*() const noexcept {return *_ptr;}
        [[nodiscard]] pointer operator->() const noexcept {return _ptr;}

        // Prefix Increment: ++a
        iterator& operator++() noexcept {
            ++_ptr;
            return *this;
        }
        // Postfix Increment: a++
        iterator operator++(int) noexcept {
            iterator before = *this;
            ++_ptr;
            return before;
        }
        // Prefix Decrement: --a
        iterator& operator--() noexcept {
            _ptr--;
            return *this;
        }
        // Postfix Decrement: a--
        iterator operator--(int) noexcept {
            iterator before = *this;
            _ptr--;
            return before;
        }

        iterator& operator+=(difference_type offset) noexcept {
            for (difference_type i=0;i<offset;i++){
                _ptr++;
            }
            return *this;
        }
        [[nodiscard]] iterator operator+(difference_type offset) const noexcept {
            iterator newIterator = *this;
            newIterator+=offset;
            return newIterator;
        }
        
        iterator& operator-=(difference_type offset) noexcept {
            for (difference_type i=0;i<offset;i++){
                _ptr--;
            }
            return *this;
        }
        [[nodiscard]] iterator operator-(difference_type offset) const noexcept {
            iterator newIterator = *this;
            newIterator-=offset;
            return newIterator;
        }
        [[nodiscard]] difference_type operator-(const iterator& rhs) const noexcept {
            difference_type distance=0;
            iterator temp = *this;
            while (temp._ptr != rhs._ptr){
                temp--;
                distance++;
            }
            return distance;
        }

        [[nodiscard]] reference operator[](difference_type offset) const noexcept {
            iterator temp = *this;
            for (difference_type i=0;i<offset;i++){
                temp++;
            }
            T* ptr = temp._ptr;
            return *ptr;
        }

        [[nodiscard]] bool operator==(const iterator& rhs) const noexcept {
            if (_ptr==rhs._ptr){return true;} return false;
        }
        [[nodiscard]] bool operator!=(const iterator& rhs) const noexcept {
            if (_ptr!=rhs._ptr){return true;} return false;
        }
        [[nodiscard]] bool operator<(const iterator& rhs) const noexcept {
            if (_ptr<rhs._ptr){return true;} return false;
        }
        [[nodiscard]] bool operator>(const iterator& rhs) const noexcept {
            if (_ptr>rhs._ptr){return true;} return false;
        }
        [[nodiscard]] bool operator<=(const iterator& rhs) const noexcept {
            if (_ptr<=rhs._ptr){return true;} return false;
        }
        [[nodiscard]] bool operator>=(const iterator& rhs) const noexcept {
            if (_ptr>=rhs._ptr){return true;} return false;
        }
    };


    void clear() noexcept {
        for (size_t i=0;i<_size;i++){
            array[i]=T();
        }
        _size = 0;
    }
};

// This ensures at compile time that the deduced argument _Iterator is a Vector<T>::iterator
// There is no way we know of to back-substitute template <typename T> for external functions
// because it leads to a non-deduced context
namespace {
    template <typename _Iterator>
    using is_vector_iterator = std::is_same<typename Vector<typename _Iterator::value_type>::iterator, _Iterator>;
}

template <typename _Iterator, bool _enable = is_vector_iterator<_Iterator>::value>
[[nodiscard]] _Iterator operator+(typename _Iterator::difference_type offset, _Iterator const& iterator) noexcept {
    _Iterator newIterator = iterator;
    newIterator+=offset;
    return newIterator;
}

#endif
