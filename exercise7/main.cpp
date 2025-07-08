#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include "task3.cpp"




int main() {
    AtomicDemo demo;
    demo.run_test();
    return 0;
}