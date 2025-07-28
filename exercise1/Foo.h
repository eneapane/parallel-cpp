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
    static inline unsigned int counter = 0;

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
        if (this != &other) {
            t = std::move(other.t);
            u = std::move(other.u);
            v = std::move(other.v);
            ++counter;
        }

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

    bool operator==(const Foo& other) const{ return other.t == t; }
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

    template<std::size_t Index>
    [[nodiscard]] constexpr auto& get()& { //second ampersand means only call on lvalues
        if constexpr (Index == 0) {
            return t;
        }
        if constexpr (Index == 1) {
            return u;
        }
        if constexpr (Index == 2) {
            return v;
        }
    }

    template<std::size_t Index>
    [[nodiscard]] constexpr const auto& get() const& { //second const means that can additionally be called with const lvalue objects
        if constexpr (Index == 0) {
            return t;
        }
        if constexpr (Index == 1) {
            return u;
        }
        if constexpr (Index == 2) {
            return v;
        }
    }

    template<std::size_t Index>
    [[nodiscard]] constexpr auto&& get()&& { //second ampersand means only call on lvalues
        if constexpr (Index == 0) {
            return std::move(t);
        }
        if constexpr (Index == 1) {
            return std::move(u);
        }
        if constexpr (Index == 2) {
            return std::move(v);
        }
    }
  };

namespace std {
    template<typename T, typename U, typename V>
    struct tuple_size<::Foo<T, U, V>> {
        static constexpr size_t value = 3;
    };
    template <typename T, typename U, typename V>
    struct tuple_element<0, ::Foo<T, U, V>> {
        using type = T;
    };
    template <typename T, typename U, typename V>
    struct tuple_element<1, ::Foo<T, U, V>> {
        using type = U;
    };
    template <typename T, typename U, typename V>
    struct tuple_element<2, ::Foo<T, U, V>> {
        using type = V;
    };
}

#endif //FOO_H
