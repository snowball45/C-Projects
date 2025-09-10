#include <cstddef>    // size_t
#include <functional> // std::hash
#include <ios>
#include <utility>    // std::pair
#include <iostream>

#include "primes.h"



template <typename Key, typename T, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>>
class UnorderedMap {
    public:

    using key_type = Key;
    using mapped_type = T;
    using const_mapped_type = const T;
    using hasher = Hash;
    using key_equal = Pred;
    using value_type = std::pair<const key_type, mapped_type>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    private:

    struct HashNode {
        HashNode *next;
        value_type val;

        HashNode(HashNode *next = nullptr) : next{next} {}
        HashNode(const value_type & val, HashNode * next = nullptr) : next { next }, val { val } { }
        HashNode(value_type && val, HashNode * next = nullptr) : next { next }, val { std::move(val) } { }
    };

    size_type _bucket_count;
    HashNode **_buckets;

    HashNode * _head;
    size_type _size;

    Hash _hash;
    key_equal _equal;

    static size_type _range_hash(size_type hash_code, size_type bucket_count) {
        return hash_code % bucket_count;
    }

    public:

    template <typename pointer_type, typename reference_type, typename _value_type>
    class basic_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = _value_type;
        using difference_type = ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

    private:
        friend class UnorderedMap<Key, T, Hash, key_equal>;
        using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

        const UnorderedMap * _map;
        HashNode * _ptr;

        explicit basic_iterator(UnorderedMap const * map, HashNode *ptr) noexcept {_map = map; _ptr = ptr;}

    public:
        basic_iterator() {_ptr = nullptr;}

        basic_iterator(const basic_iterator &) = default;
        basic_iterator(basic_iterator &&) = default;
        ~basic_iterator() = default;
        basic_iterator &operator=(const basic_iterator &) = default;
        basic_iterator &operator=(basic_iterator &&) = default;
        reference operator*() const {return _ptr->val;}
        pointer operator->() const {return &(_ptr->val);}
        basic_iterator &operator++() {
            if (_ptr) {
                if (_ptr->next) {_ptr = _ptr->next;}
                else {
                    size_t bucket = _map->_bucket(_ptr->val);
                    do {bucket++;} while (bucket < _map->_bucket_count && !_map->_buckets[bucket]);
                    _ptr = (bucket < _map->_bucket_count) ? _map->_buckets[bucket] : nullptr;
                }
            }
            return *this;
        }
        basic_iterator operator++(int) {
            basic_iterator temp = *this;
            if (_ptr) {
                if (_ptr->next) {_ptr = _ptr->next;}
                else {
                    size_t bucket = _map->_bucket(_ptr->val);
                    do {bucket++;} while (bucket < _map->_bucket_count && !_map->_buckets[bucket]);
                    _ptr = (bucket < _map->_bucket_count) ? _map->_buckets[bucket] : nullptr;
                }
            }
            return temp;
        }
        bool operator==(const basic_iterator &other) const noexcept {return _ptr == other._ptr;}
        bool operator!=(const basic_iterator &other) const noexcept {return _ptr != other._ptr;}
    };

    using iterator = basic_iterator<pointer, reference, value_type>;
    using const_iterator = basic_iterator<const_pointer, const_reference, const value_type>;

    class local_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::pair<const key_type, mapped_type>;
            using difference_type = ptrdiff_t;
            using pointer = value_type *;
            using reference = value_type &;

        private:
            friend class UnorderedMap<Key, T, Hash, key_equal>;
            using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

            HashNode * _node;

            explicit local_iterator( HashNode * node ) noexcept {_node = node;}

        public:
            local_iterator() {_node = nullptr;}

            local_iterator(const local_iterator &) = default;
            local_iterator(local_iterator &&) = default;
            ~local_iterator() = default;
            local_iterator &operator=(const local_iterator &) = default;
            local_iterator &operator=(local_iterator &&) = default;
            reference operator*() const {return _node->val; }
            pointer operator->() const {return &(_node->val);}
            local_iterator & operator++() {
                if (_node->next != nullptr){_node = _node->next;}
                else{_node = nullptr;}
                return *this;
            }
            local_iterator operator++(int) {
                local_iterator temp = *this; ++(*this); return temp;}

            bool operator==(const local_iterator &other) const noexcept {return _node == other._node;}
            bool operator!=(const local_iterator &other) const noexcept {return _node != other._node;}
    };

private:

    size_type _bucket(size_t code) const {return code % _bucket_count;}
    size_type _bucket(const Key & key) const {return _bucket(_hash(key)); }
    size_type _bucket(const value_type & val) const {return _bucket(val.first); }

    HashNode*& _find(size_type bucket, const Key & key) {
        HashNode **current = &_buckets[bucket];
        while (*current) {
            if (_equal(key, (*current)->val.first)) {return *current;}
            current = &((*current)->next);
        }
        return *current;
    }

    HashNode*& _find(const Key & key) {return _find(_bucket(key), key);}

    HashNode * _insert_into_bucket(size_type bucket, value_type && value) {
        HashNode *node = new HashNode(std::move(value));
        if (_head == nullptr || _bucket(_head->val) >= bucket) {_head = node;}
        node->next = _buckets[bucket];
        _buckets[bucket] = node;
        return node;
    }

    void _move_content(UnorderedMap & src, UnorderedMap & dst) {
        for (size_type i = 0; i < src._bucket_count; i++) {
            HashNode *node = src._buckets[i];
            while (node) {
                dst.insert(std::move(node->val));
                node = node->next;
            }
        }
    }

public:
    explicit UnorderedMap(size_type bucket_count, const Hash & hash = Hash { }, const key_equal & equal = key_equal { }) {
        _bucket_count = next_greater_prime(bucket_count);
        _buckets = new HashNode*[_bucket_count];
        for (size_type i = 0; i < _bucket_count; i++) {_buckets[i] = nullptr;}
        _head = _buckets[0]; _size = 0; _hash = hash; _equal = equal;
    }

    ~UnorderedMap() {clear(); delete _head; delete[] _buckets;}

    UnorderedMap(const UnorderedMap & other) {
        _bucket_count = other._bucket_count;
        _buckets = new HashNode*[_bucket_count];
        for (size_type i = 0; i < _bucket_count; i++) {_buckets[i] = nullptr;}
        _head = _buckets[0]; _size = 0; _hash = other._hash; _equal = other._equal;
        for (size_type i = 0; i < other._bucket_count; i++) {
            HashNode *node = other._buckets[i];
            while (node) {
                this->insert(node->val);
                node = node->next;
            }
        }
    }

    UnorderedMap(UnorderedMap && other) {
        _bucket_count = other._bucket_count;
        _buckets = new HashNode*[_bucket_count];
        for (size_type i = 0; i < _bucket_count; i++) {_buckets[i] = nullptr;}
        _head = other._head; _size = other._size; _hash = other._hash; _equal = other._equal;
        for (size_type i = 0; i < _bucket_count; i++) {_buckets[i] = other._buckets[i]; other._buckets[i] = nullptr;}
        other._head = nullptr; other._size = 0;
    }

    UnorderedMap & operator=(const UnorderedMap & other) {
        if (this != &other) {
            clear();
            delete[] _buckets;

            _bucket_count = other._bucket_count;
            _buckets = new HashNode*[_bucket_count];
            for (size_type i = 0; i < _bucket_count; i++) {_buckets[i] = nullptr;}
            _head = _buckets[0];
            _size = 0;
            _hash = other._hash;
            _equal = other._equal;

            for (size_type i = 0; i < other._bucket_count; i++) {
                HashNode *node = other._buckets[i];
                while (node) {
                    this->insert(node->val);
                    node = node->next;
                }
            }
        }
    }

    UnorderedMap & operator=(UnorderedMap && other) {
        if (this != &other) {
            clear();
            delete[] _buckets;
            _bucket_count = other._bucket_count;
            _buckets = new HashNode*[_bucket_count];
            for (size_type i = 0; i < _bucket_count; i++) {
                _buckets[i] = nullptr;
            }
            _head = other._head;
            _size = other._size;
            _hash = other._hash;
            _equal = other._equal;
            for (size_type i = 0; i < _bucket_count; i++) {
                _buckets[i] = other._buckets[i];
                other._buckets[i] = nullptr;
            }
            other._head = nullptr;
            other._size = 0;
        }
        return *this;
    }

    void clear() noexcept {
        for (size_type i = 0; i < _bucket_count; i++) {
            HashNode *node = _buckets[i];
            while (node) {
                HashNode *temp = node;
                node = node->next;
                delete temp;
            }
            _buckets[i] = nullptr;
        }
        _size = 0; _head = nullptr;
    }

    size_type size() const noexcept {return _size;}

    bool empty() const noexcept {return _size == 0;}

    size_type bucket_count() const noexcept {return _bucket_count;}

    iterator begin() {return iterator(this, _head);}
    iterator end() {return iterator(this, nullptr);}

    const_iterator cbegin() const {return const_iterator(this, _head);};
    const_iterator cend() const {return const_iterator(this, nullptr);};

    local_iterator begin(size_type n) {return local_iterator(_buckets[n]);}
    local_iterator end(size_type n) {return local_iterator(nullptr);}

    size_type bucket_size(size_type n) {
        size_type count = 0;
        HashNode *node = _buckets[n];
        while (node) {count++; node = node->next;}
        return count;
    }

    float load_factor() const {return static_cast<float>(_size) / _bucket_count;}

    size_type bucket(const Key & key) const {return _bucket(key);}

    std::pair<iterator, bool> insert(value_type && value) {
        size_type bucket = _bucket(value);
        HashNode*& node = _find(bucket, value.first);
        if (node != nullptr) {return std::make_pair(iterator(this, node), false);}
        HashNode* newNode = _insert_into_bucket(bucket, std::move(value));
        _size++;
        return std::make_pair(iterator(this, newNode), true);
    }

    std::pair<iterator, bool> insert(const value_type & value) {
        size_type bucket = _bucket(value);
        value_type copy = value;
        HashNode *&node = _find(bucket, value.first);
        if (node) {return std::make_pair(iterator(this, node), false);}
        HashNode* newNode = _insert_into_bucket(bucket, std::move(copy));
        _size++;
        return std::make_pair(iterator(this, newNode), true);
    }

    iterator find(const Key & key) {
        HashNode *&node = _find(key);
        if (node) {return iterator(this, node);}
        return end();
    }

    T& operator[](const Key & key) {
        HashNode *node = _find(key);
        if (!node) {node = _insert_into_bucket(_bucket(key), value_type(key, T()));_size++;}
        return node->val.second;
    }

    iterator erase(iterator pos) {
        HashNode *&target = _find(pos._ptr->val.first);
        iterator after = pos;
        if (_head == nullptr){return end();}
        if (pos == end()){return end();} ++after;
        if (_head == pos._ptr){_head = after._ptr;} if (target != nullptr){target = target -> next;}
        delete pos._ptr; _size--;
        return after;
    }

    size_type erase(const Key & key) {
        HashNode *&target = _find(key);
        if (!target) {return 0;}
        size_type bucket = _bucket(target->val);
        HashNode *current = _buckets[bucket];
        HashNode *prev = nullptr;
        while (current != nullptr && current != target) {prev = current; current = current->next;}
        if (current == nullptr) {return 0;}
        if (prev == nullptr) {_buckets[bucket] = current->next;}
        else {prev->next = current->next;}
        if (_head == current) {_head = current->next;}
        delete current; _size--; return 1;
    }

    template<typename KK, typename VV>
    friend void print_map(const UnorderedMap<KK, VV> & map, std::ostream & os);
};

template<typename K, typename V>
void print_map(const UnorderedMap<K, V> & map, std::ostream & os = std::cout) {
    using size_type = typename UnorderedMap<K, V>::size_type;
    using HashNode = typename UnorderedMap<K, V>::HashNode;

    for(size_type bucket = 0; bucket < map.bucket_count(); bucket++) {
        os << bucket << ": ";

        HashNode const * node = map._buckets[bucket];

        while(node) {
            os << "(" << node->val.first << ", " << node->val.second << ") ";
            node = node->next;
        }

        os << std::endl;
    }
}
