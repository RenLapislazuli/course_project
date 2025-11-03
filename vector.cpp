#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <utility>

template<typename T> struct Vector {
public:
    Vector() = default;

    explicit Vector(size_t size): _capacity(size), _size(size), _data(new T[size]) {
        for (size_t i = 0; i < size; ++i)
            _data[i] = T();
    }

    explicit Vector(size_t size, const T& value): _capacity(size), _size(size), _data(new T[size]) {
        for (size_t i = 0; i < size; ++i)
            _data[i] = value;
    }

    template<class InputIterator> Vector(InputIterator first, InputIterator last) {
        size_t size = 0;
        for (InputIterator it = first; it != last; ++it) {
            ++size;
        }
        _capacity = _size = size;
        _data = new T[size];
        size_t i = 0;
        for (InputIterator it = first; it != last; ++it) {
            _data[i] = *it;
            ++i;
        }
    }

    Vector(const Vector& other) {
        _capacity = other._capacity;
        _size = other._size;
        _data = new T[_capacity];
        for (size_t i = 0; i < _size; ++i)
            _data[i] = other._data[i];
    }

    Vector(Vector&& other) {
        other.swap(*this);
    }

    Vector(std::initializer_list<T> init) {
        Vector temp(init.begin(), init.end());
        temp.swap(*this);
    }

    ~Vector() {
        delete[] _data;
    }

    Vector& operator=(const Vector& vec) {
        if (&vec != this) {
            Vector temp(vec);
            temp.swap(*this);
        }
        return *this;
    }

    Vector& operator=(Vector&& vec) {
        Vector temp(std::move(vec));
        temp.swap(*this);
        return *this;
    }

    void assign(size_t size, const T& value) {
        Vector temp(size, value);
        temp.swap(*this);
    }

    template<class InputIterator> void assign(InputIterator first, InputIterator last) {
        Vector temp(first, last);
        temp.swap(*this);
    }

    void assign(std::initializer_list<T> init) {
        Vector temp(init.first, init.last);
        temp.swap(*this);
    }


    const T& operator[](size_t i) const {
        assert(i < _size);
        return _data[i];
    }

    T& operator[](size_t i) {
        assert(i < _size);
        return _data[i];
    }

    T& front() {
        assert(_size > 0);
        return _data[0];
    }

    const T& front() const {
        assert(_size > 0);
        return _data[0];
    }

    T& back() {
        assert(_size > 0);
        return _data[_size - 1];
    }

    const T& back() const {
        assert(_size > 0);
        return _data[_size - 1];
    }

    T* data() {
        return _data;
    }

    const T* data() const {
        return _data;
    }

    bool empty() const {
        return _size == 0;
    }

    size_t size() const {
        return _size;
    }

    void reserve(size_t capacity) {
        if (capacity > _capacity) {
            T* new_data = new T[capacity];
            for (size_t i = 0; i < _size; ++i)
                new_data[i] = _data[i];
            delete[] _data;
            _data = new_data;
            _capacity = capacity;
        }
    }

    size_t capacity() const {
        return _capacity;
    }

    void shrink_to_fit() {
        if (_size < _capacity) {
            resize(_size);
        }
    }

    void clear() {
        _size = _capacity = 0;
        delete[] _data;
        _data = nullptr;
    }

    struct Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        Iterator() : _ptr(nullptr) {}
        Iterator(T* rhs) : _ptr(rhs) {}
        Iterator(const Iterator &rhs) : _ptr(rhs._ptr) {}
        /* inline Iterator& operator=(Type* rhs) {_ptr = rhs; return *this;} */
        /* inline Iterator& operator=(const Iterator &rhs) {_ptr = rhs._ptr; return *this;} */
        inline Iterator& operator+=(difference_type rhs) {_ptr += rhs; return *this;}
        inline Iterator& operator-=(difference_type rhs) {_ptr -= rhs; return *this;}
        inline T& operator*() const {return *_ptr;}
        inline T* operator->() const {return _ptr;}
        inline T& operator[](difference_type rhs) const {return _ptr[rhs];}
        
        inline Iterator& operator++() {++_ptr; return *this;}
        inline Iterator& operator--() {--_ptr; return *this;}
        // inline Iterator operator++(int) const {Iterator tmp(*this); ++ptr; return tmp;}
        // inline Iterator operator--(int) const {Iterator tmp(*this); --ptr; return tmp;}
        /* inline Iterator operator+(const Iterator& rhs) {return Iterator(_ptr+rhs.ptr);} */
        inline difference_type operator-(const Iterator& rhs) const {return _ptr-rhs._ptr;}
        inline Iterator operator+(difference_type rhs) const {return Iterator(_ptr+rhs);}
        inline Iterator operator-(difference_type rhs) const {return Iterator(_ptr-rhs);}
        friend inline Iterator operator+(difference_type lhs, const Iterator& rhs) {return Iterator(lhs+rhs._ptr);}
        friend inline Iterator operator-(difference_type lhs, const Iterator& rhs) {return Iterator(lhs-rhs._ptr);}
        
        inline bool operator==(const Iterator& rhs) const {return _ptr == rhs._ptr;}
        inline bool operator!=(const Iterator& rhs) const {return _ptr != rhs._ptr;}
        inline bool operator>(const Iterator& rhs) const {return _ptr > rhs._ptr;}
        inline bool operator<(const Iterator& rhs) const {return _ptr < rhs._ptr;}
        inline bool operator>=(const Iterator& rhs) const {return _ptr >= rhs._ptr;}
        inline bool operator<=(const Iterator& rhs) const {return _ptr <= rhs._ptr;}
    private:
        T* _ptr;
    };

    Iterator begin() {
        return Iterator(_data);
    }

    Iterator end() {
        return Iterator(_data + _size);
    }

    void push_back(const T& value) {
        if (_size >= _capacity) {
            reserve(_size * 2 + 1);
        }
        ++_size;
        _data[_size - 1] = value;
    }

    void push_back(T&& value) {
        if (_size >= _capacity) {
            reserve(_size * 2 + 1);
        }
        ++_size;
        _data[_size - 1] = value;
    }

    template<class R> void append_back(R&& rg) {
        size_t size = 0;
        for (auto it = rg.begin(); it != rg.end(); ++it) {
            ++size;
        }
        if (_size + size > _capacity) {
            reserve(std::min(_capacity * 2 + 1, _size + size));
        }
        for (auto it = rg.begin(); it != rg.end(); ++it) {
            _data[_size] = *it;
            ++_size;
        }
    }

    void insert(Iterator pos, const T& value) {
        while (pos < end()) {
            std::swap(*pos, value);
            ++pos;
        }
        push_back(value);
    }

    template<class R> void insert_range(Iterator it, R&& rg) {
        size_t size = 0;
        for (auto it = rg.begin(); it != rg.end(); ++it) {
            ++size;
        }
        if (_size + size > _capacity) {
            auto dif = it - begin();
            reserve(std::min(_capacity * 2 + 1, _size + size));
            it = begin() + dif;
        }
        for (auto it0 = end() - 1; it0 >= it; --it0) {
            std::swap(*(it0 + size), *it0);
        }
        for (auto it0 = rg.begin(); it0 != rg.end(); ++it0, ++it) {
            *it = *it0;
        }
        _size += size;
    }

    void resize(size_t size) {
        T* new_data = new T[size];
        for (size_t i = 0; i < size; ++i) {
            if (i < _size)
                new_data[i] = _data[i];
            else
                new_data[i] = T();
        }
        _size = _capacity = size;
        delete[] _data;
        _data = new_data;
    }


    void swap(Vector& other) {
        std::swap(_data, other._data);
        std::swap(_capacity, other._capacity);
        std::swap(_size, other._size);
    }

    bool operator==(const Vector<T>& vec) const {
        for (size_t i = 0; i < _size && i < vec._size; ++i) {
            if (_data[i] != vec._data[i])
                return false;
        }
        return _size == vec._size;
    }

    bool operator!=(const Vector<T>& vec) const {
        return !(*this == vec);
    }
private:
    T* _data = nullptr;
    size_t _capacity = 0;
    size_t _size = 0;
};
