#include <atomic>
#include <iostream>
#include <ostream>
#include <thread>

class AtomicDemo {
public:
    void run_test(int iterations = 9000) {
        for (int i = 0; i < iterations; ++i) {
            reset();

            std::thread threadA(&AtomicDemo::workerA, this);
            std::thread threadB(&AtomicDemo::workerB, this);

            // Wait until both threads have signaled completion
            while (!doneA.load(std::memory_order_acquire) ||
                   !doneB.load(std::memory_order_acquire)) {
                std::this_thread::yield();
                   }

            a = 4;
            b = 6;

            threadA.join();
            threadB.join();

            if (!(a == 4 && b == 6)) {
                std::cout << "Race detected: a = " << a
                          << ", b = " << b
                          << ", iteration = " << i << std::endl;
                break;
            }
        }
    }

private:
    int a = 0;
    int b = 0;
    std::atomic<bool> doneA{false};
    std::atomic<bool> doneB{false};

    void reset() {
        a = 0;
        b = 0;
        doneA.store(false, std::memory_order_release);
        doneB.store(false, std::memory_order_release);
    }

    void workerA() {
        a = 1;
        b += 2;
        doneA.store(true, std::memory_order_release);
    }

    void workerB() {
        b = ++a;
        doneB.store(true, std::memory_order_release);
    }
};

class AtomicDemo2 {
public:
    void run_test(int iterations = 9000) {
        for (int i = 0; i < iterations; ++i) {
            reset();

            std::thread threadA(&AtomicDemo2::workerA, this);
            std::thread threadB(&AtomicDemo2::workerB, this);

            // Wait until both threads have signaled completion
            while (!doneA.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }

            a = 4;
            b = 6;

            doneMain.store(true, std::memory_order_release);

            threadA.join();
            threadB.join();

            if (!(a == 5 && b == 7)) {
                std::cout << "Race detected: a = " << a
                          << ", b = " << b
                          << ", iteration = " << i << std::endl;
                break;
            }
        }
    }

private:
    int a = 0;
    int b = 0;
    std::atomic<bool> doneA{false};
    std::atomic<bool> doneB{false};
    std::atomic<bool> doneMain{false};

    void reset() {
        a = 0;
        b = 0;
        doneMain.store(false, std::memory_order_release);
        doneB.store(false, std::memory_order_release);
        doneA.store(false, std::memory_order_release);
    }

    void workerA() {
        a = 1;
        doneA.store(true, std::memory_order_release);
        while (!doneB.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        b += 2;
    }

    void workerB() {
        while (!doneMain.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        b = ++a;
        doneB.store(true, std::memory_order_release);
    }
};


class AtomicDemo3 {
public:
    void run_test(int iterations = 9000) {
        for (int i = 0; i < iterations; ++i) {
            reset();

            std::thread threadA(&AtomicDemo3::workerA, this);
            std::thread threadB(&AtomicDemo3::workerB, this);

            // Wait until both threads have signaled completion
            while (!doneA.load(std::memory_order_acquire) || !doneB.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }

            a = 4;
            b = 6;

            doneMain.store(true, std::memory_order_release);

            threadA.join();
            threadB.join();

            if (!(a == 4 && b == 4)) {
                std::cout << "Race detected: a = " << a
                          << ", b = " << b
                          << ", iteration = " << i << std::endl;
                break;
            }
        }
    }

private:
    int a = 0;
    int b = 0;
    std::atomic<bool> doneA{false};
    std::atomic<bool> doneB{false};
    std::atomic<bool> doneMain{false};

    void reset() {
        a = 0;
        b = 0;
        doneMain.store(false, std::memory_order_release);
        doneB.store(false, std::memory_order_release);
        doneA.store(false, std::memory_order_release);
    }

    void workerA() {
        a = 1;
        b += 2;
        doneA.store(true, std::memory_order_release);
    }

    void workerB() {
        //b = ++a;
        a = a+1;
        doneB.store(true, std::memory_order_release);
        while (!doneMain.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        b = a;
    }
};