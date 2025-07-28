#include <atomic>
//
// Created by enea on 7/28/25.
//
class custom_mutex {
    std::atomic_flag flag;
    custom_mutex() : flag(ATOMIC_FLAG_INIT) {}
    public:
    void lock() {
        while (flag.test_and_set()) {
            flag.wait(true); //removing this and below is valid, but causes busy waiting
        }
    }

    void unlock() {
        flag.clear();
        flag.notify_one(); //removing this and above is valid but causes busy waiting
    }

    bool try_lock() { return !flag.test_and_set(); }
};