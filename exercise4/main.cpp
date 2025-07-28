#include <iostream>
#include "task1.cpp"

void task1() {
    std::cout << "=== Condition Variable Approach ===\n";
    CondVarApproach condVar;
    condVar.demo();

    std::cout << "\n=== Atomic Flag Approach ===\n";
    AtomicFlagApproach atomicFlag;
    atomicFlag.demo();

    std::cout << "\n == Promise/Future Approach === \n";
    PromiseFutureApproach promiseFutureApproach;
    promiseFutureApproach.demo();

    std::cout << "\n == Promise/Future Approach === \n";
    PromiseSharedFutureApproach promiseSharedFutureApproach;
    promiseSharedFutureApproach.demo();
}

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    task1();
}
