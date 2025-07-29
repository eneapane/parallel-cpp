#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include "LockFreeQueue.h"

int generateRandomInt(int min, int max) {
    static std::random_device rd;  // Hardware random number generator
    static std::mt19937 gen(rd()); // Mersenne Twister generator
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

int main() {

    std::atomic<bool> flag{false};
    LockFreeQueue<int> lock_free_queue;
    std::vector<std::thread> threads;
    threads.emplace_back([&lock_free_queue, &flag]() {
        std::atomic<int> counter(10);
        while (counter.load() > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            int data = generateRandomInt(1, 10);
            lock_free_queue.push(data);
            std::cout << "\tProducer pushed " << data << std::endl;
            counter.fetch_sub(1);
        }
        flag.store(true);
    });
    threads.emplace_back([&lock_free_queue, &flag]() {
        while (!flag.load() || !lock_free_queue.empty()) {
            std::shared_ptr<int> popped_value;
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            lock_free_queue.pop(popped_value);
            std::cout << "\t\t\tConsumer popped " << *popped_value << std::endl;
        }

    });
    for (auto& thread : threads) {
        thread.join();
    }
}
