//
// Created by enea on 6/23/25.
//

#include "ThreadSafeStack.h"

template<typename T>
ThreadSafeStack<T>::ThreadSafeStack(const ThreadSafeStack & other) {
    std::scoped_lock lock(m, other.m); // simpler than std::lock + adopt_lock
    data = other.data;
}

template<typename T>
void ThreadSafeStack<T>::push(T new_value) {
    std::lock_guard lock(m);
    data.push(new_value);
}

template<typename T>
std::shared_ptr<T> ThreadSafeStack<T>::pop() {
    std::lock_guard lock(m);
    if (data.empty()) throw std::out_of_range("ThreadSafeStack<T>::pop(): empty stack");
    std::shared_ptr<T> result = std::make_shared<T>(std::move(data.front()));
    data.pop();
    return result;
}

template<typename T>
bool ThreadSafeStack<T>::empty() const {
    std::lock_guard lock(m);
    return data.empty();
}
