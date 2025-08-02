//
// Created by enea on 8/2/25.
//

#ifndef LOCK_FREE_STACK_H
#define LOCK_FREE_STACK_H
#include <atomic>


template<typename T>
class lock_free_stack {
private:
    struct node {
        T data;
        node* next;
        explicit node(T const& data_) : data(data_), next(nullptr) {}
    };
    std::atomic<node*> head;
public:
    //thread A creates new Node and sets new_node(A)->next = head.load();
    //thread B comes in just in between and creates new_node(B)->next = head.load();
    //it then compares new_node(B)->next to head. This is true. compare_exchange_weak
    //proceeds to set head=new_node(B)
    //when thread A continues its sequenced-before progression, compare_exchange_weak
    //returns false, as new_node(A)->next == prev_head != new_node(B).
    //new_node->next = head. next while iteration compare_exchange weak return true
    //and new_node(A) is added. now we have new_node(A)->new_node(B)->init_head
    void push(T const& data) {
        node* const new_node = new node(data);
        new_node->next = head.load();
        while (!head.compare_exchange_weak(new_node->next, new_node));
    }


    //PROBLEMS:
    //1. Leaking nodes, as temporary solutions to prevent dangling pointers during concurrent access
    //2. does not work on empty list: if head==nullptr, it will cause undefined behavior as it tries to read the next pointer
    //3. exception-safety issue. if an exception is thrown when copying the return value in this method's last instruction,
    //the value is lost. This problem motivates the next, improved version of this class.
    void pop(T& result) {
        node* old_head = head.load();
        while (!head.compare_exchange_weak(old_head, old_head->next)){}
        result = old_head->data;
    }
};



#endif //LOCK_FREE_STACK_H
