//
// Created by enea on 5/11/25.
//

#ifndef FOO_H
#define FOO_H
#include <tuple>


template<typename T, typename U, typename V>
class Foo {
private:
    T t;
    U u;
    V v;

public:
     static int counter;

    Foo(const T &t, const U &u, const V &v)
        : t(t),
          u(u),
          v(v) {
        ++counter;
    }

    Foo(const T &&t, const U &&u, const V &&v) : t(t), u(u), v(v) {
        ++counter;
    }

    // explicit Foo(const std::tuple<T, U, V> tuple) : t(std::get<0>(tuple)), u(std::get<1>(tuple)), v(std::get<2>(tuple)) {
    //     ++counter;
    // }

     explicit Foo(const std::tuple<T, U, V> tuple) {
        auto& [tt, uu, vv] = tuple;
        t = tt;
        u = uu;
        v = vv;
        ++counter;
    }


    Foo(const Foo &other) : t(other.t), u(other.u), v(other.v) {
        ++counter;
    }

    Foo(Foo&& other) noexcept
    : t(std::move(other.t)), u(std::move(other.u)), v(std::move(other.v)) {
        ++counter;
    }


    Foo& operator=(Foo &&other)  noexcept {
        std::swap(t, other.t);
        std::swap(u, other.u);
        std::swap(v, other.v);
        ++counter;
        return *this;
    }

    Foo& operator=(const Foo& other) {
        if (this != &other) { // protect against self-assignment
            t = other.t;
            u = other.u;
            v = other.v;
            ++counter;
        }
        return *this;
    }

    friend bool operator==(const Foo& lhs, const Foo& rhs) {
        return lhs.t == rhs.t;
    }

    bool operator<(const Foo& rhs) const { return rhs.t < t; }
    bool operator>(const Foo& rhs) const { return rhs.t > t; }

    const T& getT() const & { return t; }
    const U& getU() const & { return u; }
    const V& getV() const & { return v; }

    // --- 2. rvalue getters (for Foo&&) ---

    T getT() && { return std::move(t); }
    U getU() && { return std::move(u); }
    V getV() && { return std::move(v); }

    static int getCounter() {
        return counter;
    }
  };


#endif //FOO_H
