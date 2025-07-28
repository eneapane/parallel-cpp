//
// Created by enea on 6/1/25.
//


#include <filesystem>
#include <fstream>
#include <vector>

#include "task1/util.h"
#include "task2/ThreadSafeCounterDict.h"
#include "task3/Utils.h"

void task1_demo() {
    size_t size = 1000000;
    std::vector<int> v = create_random_vector<int>(size);
    measureAndPrintAdvanced<int>(get_serial_sum<int>, "Serial" ,v);
    measureAndPrintAdvanced<int>(get_parallel_sum<int>, "Parallel" ,v);
}

void run_benchmark(size_t num_threads, size_t increments_per_thread, size_t key_range) {
    ThreadSafeCounterDict<int, int> counter(0, 64);

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (size_t t = 0; t < num_threads; ++t) {
        threads.emplace_back([&counter, increments_per_thread, key_range]() {
            for (size_t i = 0; i < increments_per_thread; ++i) {
                counter.increase_value_for_key(i % key_range, 1);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "Threads: " << num_threads << ", Time: " << ms << " ms" << std::endl;
}

void task2_demo() {
    const size_t increments_per_thread = 100000;
    const size_t key_range = 100;

    for (size_t num_threads : {1, 2, 4, 8, 16, 32}) {
        run_benchmark(num_threads, increments_per_thread, key_range);
    }
}


int main() {
    task1_demo();
    // std::unordered_map<std::string, int> env;
    // auto start = std::chrono::high_resolution_clock::now();
    // run_serial_tokenization(env);
    // auto end = std::chrono::high_resolution_clock::now();
    //
    // std::chrono::duration<double, std::milli> elapsed_ms = end - start;
    // std::cout << elapsed_ms.count() << " ms" << std::endl;
    // // for (const auto& [key, value] : env) {
    // //     std::cout << key << ": " << value << '\n';
    // // }
    // std::cout << env.size()<< std::endl;
    //
    //
    // ThreadSafeCounterDict<std::string, int> parallel_env(0, 8196);
    // start = std::chrono::high_resolution_clock::now();
    // run_parallel_code(parallel_env);
    // end = std::chrono::high_resolution_clock::now();
    // elapsed_ms = end - start;
    // std::cout << elapsed_ms.count() << " ms" << std::endl;
    // std::cout << parallel_env.convert_to_pairs().size() << std::endl;

}
