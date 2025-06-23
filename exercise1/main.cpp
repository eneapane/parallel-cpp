#include <cstdint>
#include <iostream>
#include <set>
#include <vector>

#include "Foo.h"

template <typename... Foos>
void print_values(const Foos&... foos) {
    // Use fold expression to print values for each Foo object in the parameter pack
    ((std::cout << "T: " << foos.getT() << ", U: " << foos.getU() << ", V: " << foos.getV() << std::endl), ...);
    std::cout << std::endl;
}

using exercise = Foo<uint32_t, std::string, std::string>;
using exerciseS = std::vector<exercise>;

bool are_sorted(const exerciseS &);
std::vector<uint32_t> get_duplicate_ids(const exerciseS &);

int main() {
    exerciseS vec;
    vec.emplace_back(1, "", "");
    vec.emplace_back(1, "", "");
    vec.emplace_back(2, "", "");
    vec.emplace_back(2, "", "");
    vec.emplace_back(3, "", "");
    vec.emplace_back(4, "", "");
    std::vector<uint32_t> duplicate_ids = get_duplicate_ids(vec);

    for (const auto &id : duplicate_ids) {
        std::cout << id << std::endl;
    }
}


bool are_sorted(const exerciseS & vec) {
    for (int i = 1; i < vec.size(); ++i) {
        bool smaller = vec[i] < vec[i - 1];
        bool equal = vec[i] == vec[i - 1];
        if (!smaller && !equal) {
            return false;
        }
    }
    return true;
}

std::vector<uint32_t> get_duplicate_ids(const exerciseS & vec) {
    if (!are_sorted(vec)) {
        throw std::invalid_argument("Invalid exercises");
    }
    std::vector<uint32_t> duplicate_ids;
    std::set<uint32_t> already_seen;
    for (const auto & i : vec) {
        if (already_seen.contains(i.getT())) {
            duplicate_ids.push_back(i.getT());
        }
        else {
            already_seen.insert(i.getT());
        }
    }
    return duplicate_ids;
}