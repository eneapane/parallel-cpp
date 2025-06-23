//
// Created by enea on 6/7/25.
//

#include "custom_synchronization.h"

#include <condition_variable>
#include <iostream>
#include <random>

bool custom_synchronization::waiting_on_signal() {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // static std::random_device rd; // Seed
    // static std::mt19937 gen(rd()); // Random number engine
    // static std::bernoulli_distribution dist(0.2); // 20% chance of true
    // return dist(gen);
    return true;
}

void custom_synchronization::supply(int supplier_id) {
    if (waiting_on_signal()) {
        {
            std::lock_guard<std::mutex> lk(mut);
            supplier_queue.push(supplier_id);
        }
        cond_var.notify_one();
    }
}

void custom_synchronization::consume(int consumer_id) {
    std::unique_lock<std::mutex> lk(mut);
    cond_var.wait(lk, [this]() { return !supplier_queue.empty(); });

    int supplier_id = supplier_queue.front();
    supplier_queue.pop();

    std::cout << "Consumer thread " << consumer_id
                << " woke up, notified by supplier thread " << supplier_id << "\n";
    lk.unlock();

    // unlock before continuing work
}
