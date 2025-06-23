#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

#include "ThreadSafeArray.h"


void testThreadSafeArray() {
    ThreadSafeArray<int, 2> arr;

    std::cout << "=== Test 1: Concurrent writes to different indices ===" << std::endl;

    std::thread writer1([&]() {
        for (int i = 0; i < 1000; ++i) {
            arr.set(0, i);
        }
    });

    std::thread writer2([&]() {
        for (int i = 1000; i < 2000; ++i) {
            arr.set(1, i);
        }
    });

    writer1.join();
    writer2.join();

    std::cout << "Final values:\n";
    std::cout << "arr[0] = " << arr.get(0) << "\n";
    std::cout << "arr[1] = " << arr.get(1) << "\n";

    std::cout << "=== Test 2: Concurrent reads of same index ===" << std::endl;
    arr.set(0, 42);  // Set a known value

    std::vector<std::thread> readers;
    for (int i = 0; i < 10; ++i) {
        readers.emplace_back([&, i]() {
            int val = arr.get(0);
            assert(val == 42); // If this fails, thread-safety is broken
            std::cout << "Reader " << i << ": arr[0] = " << val << "\n";
        });
    }

    for (auto &t : readers) {
        t.join();
    }

    std::cout << "All concurrent readers saw the correct value.\n";
}
int main() {
    testThreadSafeArray();
    return 0;
}