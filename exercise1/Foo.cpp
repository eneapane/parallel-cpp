//
// Created by enea on 5/11/25.
//

#include "Foo.h"

#include <cstdint>
#include <string>


template<typename T, typename U, typename V>
inline int Foo<T, U, V>::counter = 0;

// template class Foo<uint32_t, std::pmr::string, std::string>;
template<>
int Foo<uint32_t, std::string, std::string>::counter = 0;