#pragma once
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>


static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch) && ch != '\n';
    }));
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch) && ch != '\n';
    }).base(), s.end());
}

/**
 * Remove leading and trailing white spaces from a string
 * @param s The string (modified in-place)
 */
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

/**
 * Converts all characters in a string to lower case
 * @param s The string (modified in-place)
 */
static inline void to_lower_case(std::string & s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}
namespace fs = std::filesystem;

static void directory_iteration(fs::path& base_dir) {

}


static std::unordered_map<std::string, int>& run_serial_tokenization(std::unordered_map<std::string, int>& data) {
    fs::path base_dir = std::filesystem::path(__FILE__).parent_path().parent_path().append("20news-18828");
    for (const auto& dir_entry : fs::directory_iterator(base_dir)) {
        if (!dir_entry.is_directory()) continue;

        for (const auto &file_entry : fs::directory_iterator(dir_entry.path())) {
            if (!file_entry.is_regular_file()) continue;
            std::ifstream file(file_entry.path());
            if (!file.is_open()) {
                std::cerr << "Could not open file: " << file_entry.path() << '\n';
                continue;
            }
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string word;
                while (iss >> word) {
                    trim(word);
                    to_lower_case(word);
                    data[word]++;
                }
            }
        }
    }
    return data;
}


static void run_parallel_code(ThreadSafeCounterDict<std::string, int>& data) {

    std::vector<std::thread> threads;
    fs::path base_dir = std::filesystem::path(__FILE__).parent_path().parent_path().append("20news-18828");
    const int num_threads = 20;
    std::vector<fs::path> child_dirs;

    // Collect immediate child directories
    for (const auto& entry : fs::directory_iterator(base_dir)) {
        if (entry.is_directory()) {
            child_dirs.push_back(entry.path());
        }
    }
    for (unsigned int thread_id = 0; thread_id < num_threads; thread_id++) {
        threads.emplace_back([&data, &child_dirs, thread_id]() {
            for (const auto &file_entry : fs::directory_iterator(child_dirs[thread_id])) {
                if (!file_entry.is_regular_file()) continue;
                std::ifstream file(file_entry.path());
                if (!file.is_open()) {
                    std::cerr << "Could not open file: " << file_entry.path() << '\n';
                    continue;
                }
                std::string line;
                while (std::getline(file, line)) {
                    std::istringstream iss(line);
                    std::string word;
                    while (iss >> word) {
                        trim(word);
                        to_lower_case(word);
                        data.increase_value_for_key(word, 1);
                    }
                }
            }
        });
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }
}