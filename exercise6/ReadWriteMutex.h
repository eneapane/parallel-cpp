#ifndef READWRITEMUTEX_H
#define READWRITEMUTEX_H
#include <condition_variable>


class ReadWriteMutex {
private:
    std::mutex m;
    std::condition_variable cv;
    int readers = 0;
    bool writer_active = false;

public:
    void lock() {
        std::unique_lock lock(m);
        cv.wait(lock, [&]{ return !writer_active && readers == 0; });
        writer_active = true;
    }

    void unlock() {
        std::unique_lock lock(m);
        writer_active = false;
        cv.notify_all(); // Wake both readers and writers
    }

    void lock_shared() {
        std::unique_lock lock(m);
        cv.wait(lock, [&]{ return !writer_active; });
        ++readers;
    }

    void unlock_shared() {
        std::unique_lock lock(m);
        if (--readers == 0)
            cv.notify_all(); // Wake waiting writer
    }
};




#endif //READWRITEMUTEX_H
