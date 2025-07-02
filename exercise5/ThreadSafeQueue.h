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
    struct node {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };

    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node* tail = nullptr;
    std::condition_variable data_cond;

    node* get_tail() {
        std::lock_guard<std::mutex> lock(tail_mutex);
        return tail;
    }

    std::shared_ptr<T> pop_head() {
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

    std::unique_lock<std::mutex> wait_for_data() {
        std::unique_lock<std::mutex> head_lock(head_mutex);
        data_cond.wait(head_lock, [&] {return head.get() != get_tail(); });
        return std::move(head_lock);
    }

    std::unique_ptr<node> wait_pop_head() {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        return pop_head();
    }

    std::unique_ptr<node> try_pop_head() {
        std::lock_guard<std::mutex> lock(head_mutex);
        if (head.get() == get_tail()) {
            return std::unique_ptr<node>(nullptr);
        }
        return pop_head();
    }
public:
    ThreadSafeQueue() : head(new node), tail(head.get()) {}; //initialize with dummy node
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    [[no_discard]] std::shared_ptr<T> try_pop();
    [[no_discard]] std::shared_ptr<T> wait_and_pop();
    void push(T value);
    [[no_discard]] bool empty();
};




#endif //THREADSAFEQUEUE_H
