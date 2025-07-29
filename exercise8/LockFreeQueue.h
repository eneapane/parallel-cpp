//
// Created by enea on 7/29/25.
//

#ifndef LOCKFREEQUEUE_H
#define LOCKFREEQUEUE_H
#include <memory>


template <typename T>
class LockFreeQueue {
private:
    struct Node {
        std::shared_ptr<T> data;
        Node* next;
        Node() : next(nullptr) {}
    };
    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    Node* pop_head(){
        Node* const old_head = head.load();
        if (old_head == tail.load()){
            return nullptr;
        }
        head.store(old_head->next);
        return old_head;
    }

public:
LockFreeQueue();
LockFreeQueue(const LockFreeQueue&) = delete;
LockFreeQueue& operator=(const LockFreeQueue&) = delete;
~LockFreeQueue();
void push(const T& data);
void pop(std::shared_ptr<T>& res);

    bool empty();

};

template<typename T>
LockFreeQueue<T>::LockFreeQueue() : head(new Node{}), tail(head.load()) {
}

template<typename T>
LockFreeQueue<T>::~LockFreeQueue() {
    while (Node* const old_head = head.load()) {
        head.store(old_head->next);
        delete old_head;
    }
}

template<typename T>
void LockFreeQueue<T>::push(const T &data) {
    std::shared_ptr<T> new_data(std::make_shared<T>(data));
    Node* p = new Node;
    Node* const old_tail = tail.load();
    old_tail->data.swap(new_data);
    old_tail->next = p;
    tail.store(p);
}

template<typename T>
void LockFreeQueue<T>::pop(std::shared_ptr<T> &res) {
    Node* old_head = pop_head();
    if (!old_head) {
        res=std::shared_ptr<T>();
        return;
    }
    res=old_head->data;
    delete old_head;
}

template<typename T>
bool LockFreeQueue<T>::empty() {
    return head.load() == tail.load();
}


#endif //LOCKFREEQUEUE_H
