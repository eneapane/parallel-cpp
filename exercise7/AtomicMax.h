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
        //what the second check does
        //assuming we have 5 already stored in value
        //Thread A comes and reads current 5. It wants to store 7
        //Thread B hops in, reads 5, and stores 7. Atomic Max store finished.
        //Now we have a race condition in A, as the current it just read (5) is not what is currently there (7)
        //But that's okay
        //compare_exchange_weak will firstly check if value is equal to current. Since it is not, the loop will fail.
        //at the same time, since current is being passed by reference, compare_exchange_value will update current to be 7
        //next time the loop will not accept it.

        //Think of it like you are talking to a bombshell (value) who just broke up from her lame ex boyfriend(5)
        //You(7) have been talking to her but you are hesitant because she(value) is such a bombshell.
        //another guy more confident (7) comes about and talks to her and gets in a relationship with her.
        //next time you try talking to her, you realize that she has a new boyfriend(current has been updating)
        //other advances will be cut short from her side (newValue == current)
        while (newValue > current && !value.compare_exchange_weak(current, newValue,memory_order, memory_order)) {}
    }

    int load(std::memory_order memory_order) {
        return value.load(memory_order);
    }
};



#endif //ATOMICMAX_H
