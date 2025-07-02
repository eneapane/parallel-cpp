//
// Created by enea on 6/23/25.
//

#include "ThreadSafeQueue.h"

template<typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::try_pop() {
    std::unique_ptr<node> old_head = try_pop_head();
    return old_head ?
            old_head->data :
                std::shared_ptr<T>();
}

template<typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::wait_and_pop() {
    std::unique_ptr<node> const old_head = wait_pop_head();
    return old_head->data;

}

template<typename T>
void ThreadSafeQueue<T>::push(T value) {
    std::shared_ptr<T> new_data(std::make_shared<T>(std::move(value)));
    std::unique_ptr<node> p(new node);
    node* const new_tail = p.get();
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);

        tail->data = new_data;
        tail->next = new_tail;
        tail = new_tail; //invariant: tail points to an empty node
    }
    data_cond.notify_one();
}

template<typename T>
bool ThreadSafeQueue<T>::empty() {
    std::lock_guard<std::mutex> head_lock(head_mutex);
    return (head.get() == get_tail());
}
