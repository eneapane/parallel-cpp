//
// Created by enea on 6/23/25.
//

#ifndef THREADSAFESTACK_H
#define THREADSAFESTACK_H
#include <memory>
#include <mutex>
#include <stack>


template <typename T>
class ThreadSafeStack {
private:
    std::stack<T> data{};
    mutable std::mutex m{};
public:
    ThreadSafeStack() = default;
    ThreadSafeStack(const ThreadSafeStack& other);
    void push(T new_value);
    std::shared_ptr<T> pop();
    bool empty() const;
};



#endif //THREADSAFESTACK_H
