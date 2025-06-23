//
// Created by enea on 6/7/25.
//

#ifndef CUSTOM_MUTEX_H
#define CUSTOM_MUTEX_H
#include <atomic>


class custom_mutex {
public:
    void lock();
    bool try_lock();
    void unlock();
    //native_handle_type native_handle();
private:
    mutable std::atomic_flag flag = ATOMIC_FLAG_INIT;
};





#endif //CUSTOM_MUTEX_H
