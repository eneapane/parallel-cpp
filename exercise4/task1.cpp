#include <thread>
#include <chrono>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <future>
#include <vector>

// Abstract base class
class ProducerConsumerBase {
public:
    virtual ~ProducerConsumerBase() = default;

    // Pure virtual methods that derived classes must implement
    virtual void producer() = 0;
    virtual void consumer(int thread_id) = 0;

    // Common demo functionality
    virtual void demo() {
        std::vector<std::thread> threads;

        // Create consumer threads
        for (int i = 1; i <= getConsumerCount(); ++i) {
            threads.emplace_back(&ProducerConsumerBase::consumer, this, i);
        }

        // Create producer thread
        threads.emplace_back(&ProducerConsumerBase::producer, this);

        // Join all threads
        for (auto& t : threads) {
            t.join();
        }
    }

protected:
    // Virtual method to allow customization of consumer count
    virtual int getConsumerCount() const { return 3; }

    // Common timing utility
    void sleepForSeconds(int seconds) {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    }

    // Common timing measurement
    std::chrono::milliseconds measureDuration(
        const std::chrono::high_resolution_clock::time_point& start,
        const std::chrono::high_resolution_clock::time_point& end) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    }

    // Common output
    void printResult(int thread_id, std::chrono::milliseconds duration) {
        std::cout << thread_id << " finally got the signal after "
                  << duration.count() << " ms\n";
    }
};

// Condition Variable implementation
class CondVarApproach : public ProducerConsumerBase {
private:
    std::condition_variable cv;
    std::mutex mtx;

public:
    void producer() override {
        sleepForSeconds(2);
        cv.notify_all();
    }

    void consumer(int thread_id) override {
        auto start = std::chrono::high_resolution_clock::now();
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock);
        auto end = std::chrono::high_resolution_clock::now();

        printResult(thread_id, measureDuration(start, end));
    }
};

// Atomic Flag implementation
class AtomicFlagApproach : public ProducerConsumerBase {
private:
    std::atomic<bool> flag{false};

public:
    void producer() override {
        sleepForSeconds(2);
        flag.store(true, std::memory_order_release);
    }

    void consumer(int thread_id) override {
        auto start = std::chrono::high_resolution_clock::now();
        while (!flag.load(std::memory_order_acquire)) {
            // Optionally add small delay to reduce CPU usage
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        auto end = std::chrono::high_resolution_clock::now();

        printResult(thread_id, measureDuration(start, end));
    }

protected:
    // Atomic flag works better with fewer consumers for this demo
    int getConsumerCount() const override { return 1; }
};


class PromiseFutureApproach : public ProducerConsumerBase {
private:
    std::promise<void> promise;
    std::future<void> future;
public:
    PromiseFutureApproach() : future(promise.get_future()) {}
    void producer() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        promise.set_value();
    }

    void consumer(int thread_id) override {
        auto start = std::chrono::high_resolution_clock::now();
        future.get();
        auto end = std::chrono::high_resolution_clock::now();
        printResult(thread_id, measureDuration(start, end));
    }
protected:
    int getConsumerCount() const override { return 1; }
};

class PromiseSharedFutureApproach : public ProducerConsumerBase {
private:
    std::promise<void> promise;
    std::shared_future<void> future;
public:
    PromiseSharedFutureApproach() : future(promise.get_future().share()) {}
    void producer() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        promise.set_value();
    }

    void consumer(int thread_id) override {
        auto start = std::chrono::high_resolution_clock::now();
        future.get();
        auto end = std::chrono::high_resolution_clock::now();
        printResult(thread_id, measureDuration(start, end));
    }
protected:
    int getConsumerCount() const override { return 5; }
};