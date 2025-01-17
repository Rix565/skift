#pragma once

#include <initializer_list>
#include <type_traits>

#include <assert.h>
#include <libsystem/Common.h>

template <typename T, size_t N>
class Array
{
    static_assert(N > 0, "Array must have size greater than 0");

private:
    T _storage[N];

public:
    constexpr size_t count() const { return N; }

    constexpr T* raw_storage() { return _storage; } 
    constexpr const T *raw_storage() const { return _storage; }

    T &at(size_t index)
    {
        assert(index < N);
        return _storage[index];
    }

    const T &at(size_t index) const
    {
        assert(index < N);
        return _storage[index];
    }

    constexpr Array()
    {
    }

    constexpr Array(std::initializer_list<T> data)
    {
        static_assert(data.size() == N, "Must initialize all values of array");

        for (size_t i = 0; i < N; i++)
        {
            _storage[i] = data[i];
        }
    }

    T &operator[](size_t index)
    {
        assert(index < N);

        return _storage[index];
    }

    constexpr T &operator[](size_t index) const
    {
        assert(index < N);

        return _storage[index];
    }

    // Array iteration
    class iterator
    {
    public:
        iterator(T *ptr) : _ptr(ptr) {}
        iterator operator++()
        {
            ++_ptr;
            return *this;
        }
        bool operator!=(const iterator &other) const { return _ptr != other._ptr; }
        const T &operator*() const { return *_ptr; }

    private:
        T *_ptr;
    };

    constexpr iterator begin() const { return iterator(_storage); }
    constexpr iterator end() const { return iterator(_storage + N); }
};