#ifndef MDPC_GF4_CPP_RANDOM_H
#define MDPC_GF4_CPP_RANDOM_H

#include <algorithm>
#include <random>
#include <vector>

/**
 * @brief Random class is a singleton used to generate random integers, vectors and polynomials.
 */
class Random {
private:
    std::mt19937 engine{std::random_device{}()};
    Random() = default;

    /**
     * @brief Get the instance of Random.
     * @return Instance of Random.
     */
    static auto get() -> Random& {
        static Random instance;
        return instance;
    }

public:
    /**
     * @brief Class Random is a singleton, so copying is disallowed.
     * @param r
     */
    Random(const Random& r) = delete;

    /**
    * @brief Generate a random unsigned integer from the range [0, inclusive_bound].
    * @tparam T Unsigned integral type.
    * @param inclusive_bound Inclusive top bound.
    * @return Randomly generated unsigned integer x such that 0 <= x <= inclusive_bound.
    */
    template<typename T>
    static auto integer(T inclusive_bound) -> T {
        return std::uniform_int_distribution<T>{0, inclusive_bound}(get().engine);
    }

    /**
    * @brief Generate a random unsigned integer from the range [inclusive_low_bound, inclusive_high_bound].
    * @tparam T Unsigned integral type.
    * @param inclusive_low_bound Inclusive bottom bound.
    * @param inclusive_high_bound Inclusive top bound.
    * @return Randomly generated unsigned integer x such that inclusive_low_bound <= x <= inclusive_high_bound.
    */
    template<typename T>
    static auto integer(T inclusive_low_bound, T inclusive_high_bound) -> T {
        return std::uniform_int_distribution<T>{inclusive_low_bound, inclusive_high_bound}(get().engine);
    }
};

#endif //MDPC_GF4_CPP_RANDOM_H
