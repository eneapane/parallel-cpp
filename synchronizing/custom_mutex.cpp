//
// Created by enea on 6/7/25.
//

#include "custom_mutex.h"

void custom_mutex::lock() {
    while (flag.test_and_set(std::memory_order_acquire)) {
        flag.wait(true, std::memory_order_relaxed);
    }
}

void custom_mutex::unlock() {
    flag.clear(std::memory_order_release);
    flag.notify_one();
}

bool custom_mutex::try_lock() {
    return !flag.test_and_set(std::memory_order_acquire);
}