//
// Created by enea on 6/23/25.
//

#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H
#include <condition_variable>
#include <memory>
#include <queue>


template <typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex m;
    std::queue<T> data;
    std::condition_variable cv;
public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    [[no_discard]] std::shared_ptr<T> try_pop();
    [[no_discard]] std::shared_ptr<T> wait_and_pop();
    void push(T value);
    [[no_discard]] bool empty();
};




#endif //THREADSAFEQUEUE_H
