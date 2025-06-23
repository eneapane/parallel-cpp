#include <chrono>
#include <future>
#include <iostream>
#include <thread>

#include "custom_mutex.h"
#include "custom_synchronization.h"
#include "futures_synchronization.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
std::string oneOffTask() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return "One-off event completed";
}

void demo_async_future() {
    std::future<std::string> future = std::async(std::launch::async, oneOffTask);
    std::cout << "Waiting for one-off event..." << std::endl;
    if (future.wait_for(std::chrono::milliseconds(522)) == std::future_status::ready) {
        std::cout << future.get() << std::endl;
    }
    else {
        std::cout << "Still waiting" << std::endl;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::cout << future.get() << std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Needed to wait an additional " << duration << " milliseconds for the result." << std::endl;
}

void demo_custom_mutex() {
    custom_mutex mutex;


    int counter = 0;
    const int num_threads = 10;
    const int increments_per_thread = 10;
    auto worker = [&mutex](int thread_id) {
        std::lock_guard lock(mutex);
        for (int i = 0; i < 5; ++i) {
            std::cout << "Thread " << thread_id << " counting " << i << std::endl;
        }
    };
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Expected: " << (num_threads * increments_per_thread)
            << ", Actual: " << counter << std::endl;
}

void demo_condvar_synchronization() {
    custom_synchronization c;
    std::vector<std::thread> threads;
    for (int i = 0; i < 9; ++i) {
        if (i%2 == 0) {
            threads.emplace_back(&custom_synchronization::supply, std::ref(c), i);
        }
        else {
            threads.emplace_back(&custom_synchronization::consume, std::ref(c), i);
        }
    }

    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    //demo_async_future();
    //demo_custom_mutex();
    // demo_condvar_synchronization();
    std::vector<std::thread> threads;
    std::future<void> future;
    const int num_threads = 8;

    futures_synchronization f;
    for (int thread_id = 0; thread_id < num_threads; ++thread_id) {
        if (thread_id%2 == 0) {
            threads.emplace_back(&futures_synchronization::supply, std::ref(f), thread_id);
        }
        else {
            threads.emplace_back(&futures_synchronization::consume, std::ref(f), thread_id);
        }
    }

    for ( auto& t : threads) {
        t.join();
    }
}
