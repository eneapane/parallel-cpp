//
// Created by enea on 6/7/25.
//

#include "futures_synchronization.h"

#include <future>
#include <iostream>
#include <random>


void futures_synchronization::consume(int consumer_id){
    while (shutdown_message_received()) {
        std::packaged_task<int()> task;
        {
            std::lock_guard<std::mutex> lk(m);
            if (tasks.empty()) {
                continue;
            }
            task = std::move(tasks.front());
            tasks.pop_front();
        }
        task();
        break;


    }
}

std::future<int> futures_synchronization::supply(int supplier_id){
    auto lambda = [supplier_id, this] {
        ++this->counter;
        return supplier_id;
    };
    std::packaged_task<int()> task(lambda);
    std::future<int> future = task.get_future();
    {
        std::lock_guard<std::mutex> lk(m);
        tasks.push_back(std::move(task));
    }
    return future;
}

bool futures_synchronization::shutdown_message_received() {
    static auto start_time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();

    return elapsed < 4;  // true only during the first 4 seconds
}
