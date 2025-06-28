#include <array>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>


void dining_philosophers_demo() {
    std::vector<std::thread> threads; //each thread representing a philosopher

    std::array<std::mutex, 5> mutexes; //each mutex representing a fork
    std::array<bool, 5> philosopherAte{};

    for (int i = 0; i < mutexes.size(); i++) {
        threads.emplace_back([&mutexes, &philosopherAte, i]() {
            /*std::lock_guard<std::mutex> lock_guard(mutexes[i]); //capture the fork to the left
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); //philosophers needs time to take up the other fork
            std::lock_guard<std::mutex> lock(mutexes[(i + 1)%mutexes.size()]); //capture the fork to the right*/ // uncomment to see the false answer, which produces a deadlock
                //instead use scoped_lock and take hold of the forks in a universally defined order
            std::scoped_lock lock(mutexes[i], mutexes[(i + 1)%mutexes.size()]);
            std::this_thread::sleep_for(std::chrono::milliseconds(300)); //eat for 100 ms
            philosopherAte[i] = true;
            //forks released when thread goes out of scope
        });
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    for (int i = 0; i < mutexes.size(); i++) {
        std::cout << "Philosopher " << i << " has already eaten: " << philosopherAte[i] << std::endl;
    }
}

int main() {
    dining_philosophers_demo();
    return 0;
}