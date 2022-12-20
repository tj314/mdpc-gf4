#ifndef MDPC_GF4_RANDOM_H
#define MDPC_GF4_RANDOM_H

#include <algorithm>
#include <random>
#include <vector>
#include <cstdlib>

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

    /**
     * @brief Generate a random vector with given hamming weight.
     *
     * The nonzero elements are drawn from a GF(2^n) uniformly at random. The vector is shuffled using Fisher-Yates algorithm.
     *
     * @tparam T A finite field of type GF(2^n).
     * @param length The length of the vector.
     * @param weight The hamming weight of the vector, i. e. the number of nonzero entries.
     * @return Randomly generated vector of a given hamming weight.
     */
    template<typename T>
    static auto random_weighted_vector_over_GF2N(size_t length, size_t weight) -> std::vector<T> {
        std::vector<T> out;
        out.resize(length);
        for (size_t i = 0; i < weight; ++i) {
            T val{Random::integer<size_t>(1, T::get_max_value())};
            out[i] = val;
        }

        for (size_t i = 0; i < length; ++i) {
            size_t j = Random::integer(i, length-1);
            if (i != j) {
                std::iter_swap(out.begin() + i, out.begin() + j);
            }
        }
        return out;
    }

    /**
     * @brief Generate a random vector.
     *
     * Values are drawn from GF(2^n) uniformly at random.
     *
     * @param length
     * @return
     */
    template<typename T>
    static auto random_vector_over_GF2N(size_t length) -> std::vector<T> {
        std::vector<T> out;
        for (size_t i = 0; i < length; ++i) {
            T val{Random::integer<size_t>(0, T::get_max_value())};
            out.push_back(val);
        }
        return out;
    }
};

#endif //MDPC_GF4_RANDOM_H
