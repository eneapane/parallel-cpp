#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

template<std::integral T>
static std::vector<T> create_random_vector(const std::size_t size=1e5, const int seed = 42) {
    std::mt19937 mersenne_engine(seed);
    std::uniform_int_distribution<T> dist {1, 10};
    const auto gen = [&](){
        return dist(mersenne_engine);
    };

    std::vector<T> random_vector(size);
    std::generate(random_vector.begin(), random_vector.end(), gen);

    return random_vector;
}

template<typename T>
static void print_vector(const std::vector<T> &vector) {
    for (const auto &i: vector) {
        std::cout << i << " ";
    }
    std::cout << "\n";
}


template<typename T>
static T get_serial_sum(const std::vector<T> &vector) {
    T result = 0;
    for (int i = 0; i < vector.size(); ++i) {
        result += vector[i];
    }
    return result;
}



template<typename T>
static T get_parallel_sum(const std::vector<T> &vector) {
    const auto num_threads = std::thread::hardware_concurrency();
    std::vector<T> partial_sums(num_threads, 0); // Initialize with zeros
    std::vector<std::thread> threads;

    // Divide work among threads
    for (unsigned int thread_id = 0; thread_id < num_threads; ++thread_id) {
        threads.emplace_back(
            [&vector, &partial_sums, thread_id, num_threads]() {
                // Calculate the chunk of work for this thread
                const size_t start = (thread_id * vector.size()) / num_threads;
                const size_t end = ((thread_id + 1) * vector.size()) / num_threads;

                // Compute partial sum for this chunk
                T local_sum = 0;
                for (size_t i = start; i < end; ++i) {
                    local_sum += vector[i];
                }
                partial_sums[thread_id] = local_sum;
            }
        );
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    // Combine partial sums
    T result = 0;
    for (const auto& sum : partial_sums) {
        result += sum;
    }
    return result;
}


template <typename T>
void measureAndPrintAdvanced(std::function<T(const std::vector<T>&)> func,
                            const std::string& name,
                            const std::vector<T>& vec) {
    auto start = std::chrono::high_resolution_clock::now();
    T result = func(vec);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << name << ": " << duration.count() << " μs | Result: " << result << "\n";
}
