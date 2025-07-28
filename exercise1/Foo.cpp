//
// Created by enea on 5/11/25.
//

#include "Foo.h"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


using exercise_t = Foo<uint, std::string, std::string>;
using exercises = std::vector<exercise_t>;

bool are_sorted(const exercises& exs) {
    for (int i = 0; i < exs.size() - 1; ++i) {
        if (exs[i] > exs[i + 1]) {
            return false;
        }
    }
    return true;
}
std::vector<uint> get_duplicate_ids(const exercises& exs) {
    if (!are_sorted(exs))
        throw std::runtime_error("Not sorted");

    std::unordered_set<uint> seen{};
    std::unordered_set<uint> duplicates{};

    for (const auto& ex : exs) {
        if (!seen.insert(ex.getT()).second) {
            duplicates.insert(ex.getT());
        }
    }

    return {duplicates.begin(), duplicates.end()};
}

void sort(std::vector<exercises>& exs) {
    std::sort(exs.begin(), exs.end());
}


