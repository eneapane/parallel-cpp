//
// Created by enea on 7/4/25.
//

#ifndef ATOMICMAX_H
#define ATOMICMAX_H
#include <atomic>


class AtomicMax {
private:
    std::atomic<int> value;

    public:
    AtomicMax() : value(INT_MIN) {}

    void store(int newValue, std::memory_order memory_order) {
        int current = value.load(std::memory_order_relaxed);
        while (current != value && value.compare_exchange_weak(current, newValue,std::memory_order_release, std::memory_order_relaxed)) {}
    }

    int load(std::memory_order memory_order) {
        return value.load(memory_order);
    }
};



#endif //ATOMICMAX_H
