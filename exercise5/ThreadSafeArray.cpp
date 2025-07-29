#include "ThreadSafeArray.h"

#include <shared_mutex>


template class ThreadSafeArray<int, 2>;


template<typename T, std::size_t N>
void ThreadSafeArray<T, N>::set(std::size_t index, const T &value) {
    std::lock_guard<std::mutex> lock(mutexes[index]);
    data[index] = value;
}

template<typename T, std::size_t N>
T ThreadSafeArray<T, N>::get(std::size_t index) const {
    std::shared_lock<std::mutex> lock(mutexes[index]);
    return data[index];
}

template<typename T, std::size_t N>
T ThreadSafeArray<T, N>::operator[](std::size_t index) const {
    return get(index);
}

template<typename T, std::size_t N>
std::size_t ThreadSafeArray<T, N>::size() const {
    return N;
}
