#pragma once
#include <string>


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