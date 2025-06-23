//
// Created by enea on 6/23/25.
//

#include "ThreadSafeQueue.h"

template<typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::try_pop() {
    std::lock_guard<std::mutex> lock_guard(m);
    if (data.empty()) {
        return std::shared_ptr<T>();
    }
    std::shared_ptr<T> result = std::make_shared<T>(std::move(data.front()));
    data.pop();
    return result;
}

template<typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::wait_and_pop() {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&] -> bool { return !data.empty(); });
    std::shared_ptr<T> result = std::make_shared<T>(std::move(data.front()));
    data.pop();
    return result;
}

template<typename T>
void ThreadSafeQueue<T>::push(T value) {
    {
        std::lock_guard lock_guard(m);
        data.push(value);
    }
    cv.notify_one();
}

template<typename T>
bool ThreadSafeQueue<T>::empty() {
    return data.empty();
}
