//
// Created by enea on 6/7/25.
//

#ifndef FUTURES_SYNCHRONIZATION_H
#define FUTURES_SYNCHRONIZATION_H
#include <deque>
#include <future>
#include <mutex>


class futures_synchronization {
public:
    std::future<int> supply(int supplier_id);

    void consume(int consumer_id);

private:
    static bool shutdown_message_received();

    mutable std::mutex m;
    std::deque<std::packaged_task<int()>> tasks;
    std::atomic_int counter = 0; //to illustrate that packaged tasks serve to do something besides exchanging data
};



#endif //FUTURES_SYNCHRONIZATION_H
