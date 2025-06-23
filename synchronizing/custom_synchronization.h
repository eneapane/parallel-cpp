//
// Created by enea on 6/7/25.
//

#ifndef CUSTOM_SYNCHRONIZATION_H
#define CUSTOM_SYNCHRONIZATION_H
#include <condition_variable>
#include <queue>


class custom_synchronization {
public:
    static bool waiting_on_signal();

    void supply(int supplier_id);

    void consume(int consumer_id);

private:
    std::mutex mut;
    std::condition_variable cond_var;

    // Queue to transfer supplier IDs for consumer to read
    std::queue<int> supplier_queue;
};


#endif //CUSTOM_SYNCHRONIZATION_H
