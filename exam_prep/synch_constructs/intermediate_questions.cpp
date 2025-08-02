#include <any>
#include <array>
#include <functional>
#include <future>
#include <iostream>
#include <vector>
#include <latch>
#include <queue>
#include <random>
//Q2.1 Create a std::packaged_task that wraps a lambda function, then execute it manually.
void packaged_task_with_lambda() {
    // Wrap this lambda: [](int x, int y) { return x + y; }
    std::packaged_task<int(int, int)> task([](int x, int y) { return x + y; });
    std::future<int> future = task.get_future();
    task(33, 36); //execute in the same thread
    std::cout << future.get() << std::endl;
}


//Q2.2 Store multiple std::packaged_task objects in a vector, then execute them one by one.
void packaged_tasks_in_vector() {
    std::vector<std::packaged_task<void()>> tasks;

    auto task1 = std::make_shared<std::packaged_task<int(int, int)>>([](int x, int y) {return x + y;});
    auto task2 = std::make_shared<std::packaged_task<std::string(const std::string)>>([](const std::string& y) {return y + "!";});
    auto task3 = std::make_shared<std::packaged_task<void()>>([](){std::cout << "or pajdhsome se sum njef mir kush jom or koc" << std::endl; });

    tasks.emplace_back([task1]() {(*task1)(3, 5);});
    tasks.emplace_back([task2]() {(*task2)("Pidhi i mire");});
    tasks.emplace_back([task3]() {(*task3)();});

    for (auto& task : tasks) {
        task(); //cannot retrieve return values. If you want to retrieve them, store the appropriate futures in std::vector<std::any> futures;
    }
}

//Q2.3 Create a packaged_task, get its future, move the task to a thread, and retrieve the result.
void packaged_task_with_future() {
    std::packaged_task<int(int, int)> task([](int x, int y) { return x + y; });
    std::future<int> future = task.get_future();
    std::thread(std::move(task), 33, 36).detach();
    std::cout << future.get() << std::endl;
}


//Q2.6 Create a "result aggregator" that waits for 3 different async computations to complete and returns their sum.
void aggregate_sub_sums() {
    std::vector <int> sums{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    std::vector<std::thread> threads;
    constexpr unsigned int num_threads = 8;
    std::array<int, num_threads> partial_sums{};
    std::latch done{num_threads};
    for (unsigned i = 0; i < num_threads; i++) {
        size_t begin = i * (sums.size() / num_threads);
        size_t end = (i == num_threads - 1) ? sums.size() : (i + 1) * (sums.size() / num_threads);

        threads.emplace_back([&sums, &done, &partial_sums](size_t begin, size_t end, int thread_num) {
            int result = 0;
            for (size_t i = begin; i < end; i++) {
                result += sums[i];
            }
            partial_sums[thread_num] = result;
            done.count_down();
        }, begin, end, i);
    }
    done.wait();
    int sum = 0;
    for (int i = 0; i < num_threads; i++) {
        threads[i].join();
        sum += partial_sums[i];
    }
    std::cout <<"The required sum is "<< sum << std::endl;
}


void aggregate_sub_sums_with_futures() {
    std::vector <int> sums{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    std::vector<std::future<int>> futures;
    constexpr unsigned int num_threads = 8;
    for (unsigned i = 0; i < num_threads; i++) {
        size_t begin = i * (sums.size() / num_threads);
        size_t end = (i == num_threads - 1) ? sums.size() : (i + 1) * (sums.size() / num_threads);

        futures.emplace_back(std::async(std::launch::async, [&sums](int begin, int end) {
            int partial_sum = 0;
            for (size_t i = begin; i < end; i++) {
                partial_sum += sums[i];
            }
            return partial_sum;
        }, begin, end));
    }
    int result = 0;
    for (auto &f : futures) {
        result += f.get();
    }
    std::cout <<"The required sum is "<< result << std::endl;
}

int timeout_in_3_seconds() {
    int default_value = -1;

    std::future<int> future = std::async(std::launch::async, [](int x) {
        // Use proper random number generation with limited range
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 5);  // 1-5 seconds

        int wait = dis(gen);
        std::cout << "Waiting for " << wait << " seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(wait));
        std::cout << "Computation completed!" << std::endl;
        return x * x;
    }, 5);

    if (future.wait_for(std::chrono::seconds(3)) == std::future_status::timeout) {
        std::cout << "Timeout! Returning default value: " << default_value << std::endl;
        return default_value;
    } else {
        int result = future.get();
        std::cout << "Computation result: " << result << std::endl;
        return result;
    }
}

//Q2.9 Create a shared_future and have 3 different threads all wait for the same result.
void shared_future() {
    std::vector<std::thread> threads;
    std::promise<int> promise;
    std::shared_future<int> future = promise.get_future().share();
    for (int i = 0; i < 3; i++)
        threads.emplace_back( [&future](int thread_num) {
            std::cout << "Thread " << thread_num << " received " << future.get() << std::endl;
        }, i);
    promise.set_value(42);
    for (auto& thread : threads) {
        thread.join();
    }
}

//Q3.1 Create a promise that might set either a value or an exception based on input validation.
std::future<std::string> promise_with_exception_handling(bool exception) {
    std::promise<std::string> promise;
    std::future<std::string> future = promise.get_future();
    if (exception)
        promise.set_exception(std::make_exception_ptr(std::runtime_error("nicht alles fit leider")));
    else
        promise.set_value("Alles fit");
    return future;
}

//Q3.5 Implement a producer-consumer pattern using promises where:
//Producer generates data over time
//Multiple consumers wait for data
// Use shared_future for broadcasting
// class ProducerConsumer {
// private:
//     std::atomic<int> counter{5};
//     std::atomic<std::shared_ptr<std::shared_future<int>>> atomic_future;
//     std::atomic<bool> stop_flag{false};
//
// public:
//     void produce() {
//         while (counter.load() > 0) {
//             std::this_thread::sleep_for(std::chrono::seconds(1));
//
//             // Create new promise/future
//             auto promise = std::make_unique<std::promise<int>>();
//             auto shared_future = std::make_shared<std::shared_future<int>>(
//                 promise->get_future().share()
//             );
//
//             // Atomically update the shared future
//             atomic_future.store(shared_future);
//
//             // Set the value
//             promise->set_value(rand() % 11);
//             counter.fetch_sub(1);
//         }
//         stop_flag = true;
//     }
//
//     void consume(int i) {
//         while (!stop_flag) {
//             auto current_future = atomic_future.load();
//             if (current_future) {
//                 try {
//                     int value = current_future->get();
//                     std::cout << value << " from thread " << i << std::endl;
//                 } catch (...) {
//                     // Handle exceptions
//                 }
//             }
//             std::this_thread::sleep_for(std::chrono::milliseconds(100));
//         }
//     }
//
//     void demo() {
//         std::vector<std::thread> threads;
//         for (int i = 0; i < 3; i++)
//             threads.emplace_back(&ProducerConsumer::consume, this, i);
//
//         produce();
//
//         for (auto& thread : threads) {
//             thread.join();
//         }
//     }
// };
//Q3.6 Create a "pipeline" where output of one async operation becomes input to the next.
void pipeline_with_async() {
    double cellA = 5.0;
    double cellB = 4.5;
    std::future<double> cellCFuture = std::async(std::launch::async, [](double x, double y) {
        return x + y;
    }, cellA, cellB);
    std::future<double> cellDFuture = std::async(std::launch::async, [](double x, double y) {
        return x*y;
    }, cellA, cellB);
    std::future<double> cellEFuture = std::async(std::launch::async, [](double x, double y) {
        return x - y;
    }, cellA, cellCFuture.get());
    std::future<double> cellFFuture = std::async(std::launch::async, [](double x, double y) {
        return y - x;
    }, cellA, cellDFuture.get());
    double cellGFuture = cellEFuture.get() + cellFFuture.get();
    std::cout << cellGFuture << std::endl;
}

//Q3.7 Implement a "fan-out, fan-in" pattern: distribute work to multiple threads, then collect results.
void fan_in_fan_out_array_sum() {
    const int ARRAY_SIZE = 1000;
    std::array<int, ARRAY_SIZE> arr{};
    std::fill(arr.begin(), arr.end(), 42);
    std::vector<std::future<int>> futures;
    std::vector<std::thread> threads;
    int result = 0;
    //main thread is the main thread
    unsigned int num_threads = std::thread::hardware_concurrency();
    for (int i = 0; i < num_threads; i++) {
        int end = i == num_threads - 1 ? ARRAY_SIZE : (i + 1) * (ARRAY_SIZE / num_threads);
        int begin = i * (ARRAY_SIZE / num_threads);
        auto task = std::packaged_task<int(int)>([&arr, begin, end](int thread_num) {
            int partial_sum = 0;
            for (int j = begin; j < end; j++) {
                partial_sum += arr[j];
            }
            return partial_sum;
        });
        futures.emplace_back(task.get_future());
        threads.emplace_back(std::move(task), i);
    }
    for (auto& future : futures) {
        result += future.get(); // Wait for each thread to complete
    }

    // Clean up threads
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << result << std::endl;
}
//Q3.8 Build a simple task scheduler that can:
// • Queue packaged_tasks for later execution
// • Execute tasks in priority order
// • Return futures for results
class TaskScheduler {
private:
    // Task wrapper that holds priority and the actual task
    struct Task {
        int priority;
        std::function<void()> task;

        Task(int p, std::function<void()> t) : priority(p), task(std::move(t)) {}

        // Higher priority values execute first
        bool operator<(const Task& other) const {
            return priority < other.priority;  // Note: reversed for max-heap
        }
    };

    std::priority_queue<Task> task_queue;
    mutable std::mutex queue_mutex;
    std::condition_variable cv;
    std::atomic<bool> shutdown{false};
    std::vector<std::thread> worker_threads;

    // Worker thread function
    void worker() {
        while (true) {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // Wait for tasks or shutdown signal
            cv.wait(lock, [this]() {
                return !task_queue.empty() || shutdown.load();
            });

            if (shutdown.load() && task_queue.empty()) {
                break;
            }

            if (!task_queue.empty()) {
                Task current_task = std::move(const_cast<Task&>(task_queue.top()));
                task_queue.pop();
                lock.unlock();

                // Execute the task
                current_task.task();
            }
        }
    }

public:
    // Constructor - starts worker threads
    explicit TaskScheduler(size_t num_threads = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < num_threads; ++i) {
            worker_threads.emplace_back(&TaskScheduler::worker, this);
        }
    }

    // Destructor - cleanup
    ~TaskScheduler() {
        shutdown.store(true);
        cv.notify_all();

        for (auto& thread : worker_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    // Schedule a packaged_task with priority
    template<typename F, typename... Args>
    auto schedule_task(int priority, F&& func, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type> {

        using return_type = typename std::result_of<F(Args...)>::type;

        // Create packaged_task
        auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(func), std::forward<Args>(args)...)
        );

        // Get future before moving the task
        std::future<return_type> future = task_ptr->get_future();

        // Wrap the packaged_task in a lambda
        auto wrapper = [task_ptr]() {
            (*task_ptr)();
        };

        // Add to priority queue
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            task_queue.emplace(priority, std::move(wrapper));
        }
        cv.notify_one();

        return future;
    }

    // Get number of pending tasks
    size_t pending_tasks() const {
        std::lock_guard<std::mutex> lock(queue_mutex);
        return task_queue.size();
    }

    // Wait for all tasks to complete
    void wait_for_all() {
        while (pending_tasks() > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
};

void task_scheduler_demo() {
    TaskScheduler scheduler(4);  // 4 worker threads

    std::cout << "=== Task Scheduler Demo ===" << std::endl;

    // Example functions to schedule
    auto compute_task = [](int id, int duration) -> int {
        std::cout << "Task " << id << " starting (duration: " << duration << "ms)" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
        std::cout << "Task " << id << " completed!" << std::endl;
        return id * 100;
    };

    auto string_task = [](const std::string& msg) -> std::string {
        std::cout << "Processing: " << msg << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return "Result: " + msg;
    };

    // Schedule tasks with different priorities (higher number = higher priority)
    std::vector<std::future<int>> int_futures;
    std::vector<std::future<std::string>> string_futures;

    std::cout << "\nScheduling tasks..." << std::endl;

    // Schedule some integer tasks with different priorities
    int_futures.push_back(scheduler.schedule_task(1, compute_task, 1, 300));  // Low priority
    int_futures.push_back(scheduler.schedule_task(5, compute_task, 2, 200));  // High priority
    int_futures.push_back(scheduler.schedule_task(3, compute_task, 3, 150));  // Medium priority
    int_futures.push_back(scheduler.schedule_task(5, compute_task, 4, 100));  // High priority
    int_futures.push_back(scheduler.schedule_task(1, compute_task, 5, 250));  // Low priority

    // Schedule some string tasks
    string_futures.push_back(scheduler.schedule_task(4, string_task, "High priority message"));
    string_futures.push_back(scheduler.schedule_task(2, string_task, "Low priority message"));
    string_futures.push_back(scheduler.schedule_task(4, string_task, "Another high priority"));

    std::cout << "Tasks scheduled. Pending tasks: " << scheduler.pending_tasks() << std::endl;
    std::cout << "\nExecuting tasks in priority order...\n" << std::endl;

    // Collect results
    std::cout << "=== Integer Task Results ===" << std::endl;
    for (auto& future : int_futures) {
        try {
            int result = future.get();
            std::cout << "Got result: " << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Task failed: " << e.what() << std::endl;
        }
    }

    std::cout << "\n=== String Task Results ===" << std::endl;
    for (auto& future : string_futures) {
        try {
            std::string result = future.get();
            std::cout << "Got result: " << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Task failed: " << e.what() << std::endl;
        }
    }

    // Wait for any remaining tasks
    scheduler.wait_for_all();
    std::cout << "\nAll tasks completed!" << std::endl;

    // Demonstrate exception handling
    std::cout << "\n=== Exception Handling Demo ===" << std::endl;

    auto failing_task = []() -> int {
        std::cout << "This task will throw an exception" << std::endl;
        throw std::runtime_error("Task failed intentionally");
        return 42;
    };

    auto exception_future = scheduler.schedule_task(10, failing_task);

    try {
        int result = exception_future.get();
        std::cout << "Unexpected success: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Caught expected exception: " << e.what() << std::endl;
    }
}

void demo_intermediate_questions() {
    //packaged_task_with_lambda();
    //packaged_tasks_in_vector();
    //packaged_task_with_future();
    // aggregate_sub_sums();
    // aggregate_sub_sums_with_futures();
    //timeout_in_3_seconds();
    //shared_future();
    //std::cout << promise_with_exception_handling(true).get();
    // pipeline_with_async();
    //fan_in_fan_out_array_sum();
    task_scheduler_demo();
}