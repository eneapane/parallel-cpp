//
// Created by enea on 6/23/25.
//

#ifndef THREADSAFEARRAY_H
#define THREADSAFEARRAY_H
#include <array>
#include <cstddef>
#include <mutex>


template<typename T, std::size_t N>
class ThreadSafeArray {
private:
    mutable std::array<std::mutex, N> mutexes;
    std::array<T, N> data;
public:
    ThreadSafeArray() = default;
    ThreadSafeArray(const ThreadSafeArray &) = delete;
    ThreadSafeArray &operator=(const ThreadSafeArray &) = delete;
    void set(std::size_t index, const T &value);
    T get(std::size_t index) const;
    T operator[](std::size_t index) const;
    std::size_t size() const;
};


#endif //THREADSAFEARRAY_H
