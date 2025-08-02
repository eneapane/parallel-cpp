#include <array>
#include <cassert>
#include <iostream>
#include <thread>
//
// Created by enea on 7/29/25.
//
//Create two atomic booleans x and y. Thread 1 stores true to x then y (relaxed ordering). Thread 2 waits
//for y to be true, then checks x. Run multiple times to observe cases where x might still be false
void relaxed_ordering_demo() {
    std::atomic<bool> x{false};
    std::atomic<bool> y{false};
    for (int i = 0; i < 10000; i++) {
        //multiple demonstrations
        std::thread thread1([&x, &y]() {
            x.store(true, std::memory_order_relaxed);
            y.store(true, std::memory_order_relaxed);
        });
        std::thread thread2([&y, &x](int i) {
            while (!y.load(std::memory_order_relaxed)) {
            }
            if (!x.load(std::memory_order_relaxed)) {
                std::cout << "Iteration " << i << std::endl;
            }
        }, i);
        thread1.join();
        thread2.join();
        x.store(false);
        y.store(false);
    }
}

//
//Implement a simple producer-consumer where the producer fills an array and sets a flag with release
//semantics. The consumer waits for the flag with acquire semantics then reads the array.
void acquire_release_sync() {
    std::array<int, 10> a{};
    std::atomic<bool> flag{false};
    std::thread thread([&a, &flag]() {
        for (int i = 0; i < 10; i++) {
            a[i] = i;
        }
        flag.store(true, std::memory_order_release);
    });
    std::thread thread2([&a, &flag]() {
        while (!flag.load(std::memory_order_acquire)) {}
        for (int i = 0; i < 10; i++) {
            std::cout << a[i] << std::endl;
        }
    });
    thread.join();
    thread2.join();
}