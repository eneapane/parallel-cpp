#include <future>
#include <iostream>


//Q1.1 Write a function that uses std::async to calculate the square of a number in a separate thread.
std::future<int> async_square(int n) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return std::async(std::launch::async, [n]() {return n * n;});
}


//Q1.2 Use std::async to run two functions in parallel: add(3, 4) and multiply(5, 6) . Print both results.
int simple_addition(int a, int b) {return a + b;}
int simple_multiplication(int a, int b) {return a * b;}
void run_and_print() {
    std::future<int> addition_future = std::async(std::launch::async, simple_addition, 3, 4);
    std::future<int> multiplication_future = std::async(std::launch::async, simple_multiplication, 3, 4);
    std::cout << "The addition returned " << addition_future.get() << std::endl;
    std::cout << "The multiplication returned " << multiplication_future.get() << std::endl;
}


//Q1.3 Create an async task that sleeps for 2 seconds then returns the string "Hello". Use both
//std::launch::async and std::launch::deferred and observe the difference.
void sleepy_hello_world(bool deferred) {
    auto start = std::chrono::high_resolution_clock::now();

    std::future<std::string> result;
    if (deferred)
        result = std::async(std::launch::deferred, []() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return std::string("Hello world");
        });
    else
        result = std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return std::string("Hello world");
        });
    std::this_thread::sleep_for(std::chrono::seconds(1)); //introduce latency to demonstrate difference between deferred and async
    std::string resultUnpacked = result.get();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << resultUnpacked << " in " << duration.count() << " ms" << std::endl;
}

// Q1.5 Given a std::future<int> , write code that checks if the result is ready without blocking.
template<typename T>
bool is_future_ready(std::future<T>& future) {
    if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        return true;
    return false;
}


//Q1.6 Create a future that will timeout after 1 second if the result isn't ready. Handle the timeout case.
//should return true
template<typename T>
bool timeout_after_1_second(std::future<T>& future, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
    std::future<int> result = async_square(5);
    auto status = future.wait_for(timeout);
    if (status == std::future_status::timeout)
        return true;
    return false;
}


//Q1.7 Write a function that returns a future, then demonstrate calling .get() multiple times (and handle
//the error).
void calling_future_multiple_times() {
    std::future<int> result = async_square(5);
    //first future call
    try {
        int i1 = result.get();
        int i2 = result.get();
    } catch (std::future_error& e) {
        std::cout << e.what() << std::endl;
    }
}

//Q1.8 Create a promise/future pair where the promise sets the value 42 after 1 second delay.
void set_promise_value_after_delay() {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    promise.set_value(simple_multiplication(3, 4));
    std::cout<< "In set promise value after delay " << future.get() << std::endl;
}

void promise_sets_exception() {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    promise.set_exception(std::make_exception_ptr(std::runtime_error("Something went wrong")));
    try {
        int i1 = future.get();
        std::cout << "Trying to output i1: " << i1 << std::endl;
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

void thread_wide_promise_setting() {
    //create promise in one thread
    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    std::thread t1([&promise]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        promise.set_value(69);
    });
    t1.detach();
    std::cout << "Value in the main thread is " << future.get() << std::endl;
}


void demo_easy_questions() {
    // calling_future_multiple_times();
    // sleepy_hello_world(true);
    // sleepy_hello_world(false);
    // set_promise_value_after_delay();
    // promise_sets_exception();
    thread_wide_promise_setting();
}
