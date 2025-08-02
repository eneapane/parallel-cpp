#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
//
// Created by enea on 7/29/25.
//

// Exercise 1: Create a program with two threads: one incrementing an atomic counter 1000 times, another
// decrementing it 1000 times. Verify the final value is 0
void basic_atomic_operation() {
    //this specific version never lets counter go below 0
    std::atomic<int> counter(0);
    std::thread increment([&counter]() {
        for (int i = 0; i < 1000; i++) {
            counter.fetch_add(1, std::memory_order_release);
            counter.notify_one();
        }
    });
    std::thread decrement([&counter]() {
        for (int i = 0; i < 1000; i++) {
            // Wait until counter is greater than 0
            counter.wait(0, std::memory_order_acquire); // Wait while counter == 0

            // Now decrement (with double-check)
            int expected = counter.load(std::memory_order_acquire);
            while (expected > 0 &&
                   !counter.compare_exchange_weak(expected, expected - 1,
                                                  std::memory_order_release)) {
                //     Retry if CAS failed
                //it may fail. imagine counter is 0. two decrementers are lurking and waiting. as soon as one
                //incrementer adds 1, they both subtract one, making the counter go below -1
            }
        }
    });
    increment.join();
    decrement.join();
    std::cout << "Final value is : " << counter.load() << std::endl; //should be 0
}

void basic_atomic_operation_simpler() {
    std::atomic<int> counter(0);
    std::thread increment([&counter]() {
        for (int i = 0; i < 1000; i++) {
            counter.fetch_add(1, std::memory_order_release);
        }
    });
    std::thread decrement([&counter]() {
        for (int i = 0; i < 1000; i++) {
            // Wait until counter is greater than 0
            counter.fetch_sub(1, std::memory_order_acquire); // Wait while counter == 0
        }
    });
    increment.join();
    decrement.join();
    std::cout << "Final value is : " << counter.load() << std::endl; //should be 0
}

//Implement a spinlock using std::atomic_flag . Use it to protect a shared integer that 4 threads increment
// 1000 times each.  The final value should be 4000.
class Spinlock {
private:
    std::atomic_flag m = ATOMIC_FLAG_INIT;

public:
    void lock() {
        while (m.test_and_set()) {
        }
    }

    void unlock() {
        m.clear();
    }

    void try_lock() {
        !m.test_and_set();
    }
};

void atomic_flag_spinlock() {
    Spinlock lock;
    std::vector<std::thread> threads;
    int shared_int = 0;
    for (int i = 0; i < 4; i++) {
        threads.emplace_back([&lock, &shared_int]() {
            std::lock_guard<Spinlock> lock_guard(lock);
            for (int i = 0; i < 1000; i++) {
                shared_int++;
            }
        });
    }
    for (auto &thread: threads) {
        thread.join();
    }
    std::cout << "Final value should be : " << shared_int << std::endl;
}

// Implement an atomic increment function using compare_exchange_weak() in a loop. Test with 10 threads
// each incrementing 1000 times.
void increment(std::atomic<int> &toIncrement) {
    int expected = toIncrement.load(std::memory_order_acquire);
    while (!toIncrement.compare_exchange_weak(expected, expected + 1, std::memory_order_release));
}

void compare_and_exchange() {
    std::atomic<int> shared_int = 0;
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&shared_int]() {
            for (int i = 0; i < 1000; i++) {
                increment(shared_int);
            }
        });
    }
    for (auto &thread: threads) {
        thread.join();
    }
    std::cout << "Final value should be : " << shared_int << std::endl;
}

//Exercise 5:
// Create an atomic pointer to an integer. One thread allocates and stores new integers, another thread
// reads and processes them. Use exchange() to swap pointers safely.
void atomic_pointer_operations() {
    std::atomic<std::shared_ptr<int>> atomic_pointer(nullptr);
    std::thread alloc([&atomic_pointer]() {
        for (int i = 0; i < 10; i++) {
            std::shared_ptr<int> new_data = std::make_shared<int>(i);  // Allocate on heap
            atomic_pointer.store(new_data, std::memory_order_release);
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Give processor time
        }
        std::shared_ptr<int> end_marker = std::make_shared<int>(-1);
        atomic_pointer.store(end_marker, std::memory_order_release);
    });

    std::thread processor([&atomic_pointer]() {
        while (true) {
            // Use exchange to safely swap out the pointer
            std::shared_ptr<int> current = atomic_pointer.exchange(nullptr, std::memory_order_acq_rel);
            if (current) {
                if (*current == -1) {

                    break;
                }
                std::cout << *current << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    alloc.join();
    processor.join();
}
