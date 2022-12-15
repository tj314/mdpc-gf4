#ifndef MDPC_GF4_CPP_GF4_H
#define MDPC_GF4_CPP_GF4_H

#include <string>
#include <vector>
#include "custom_exceptions.h"
#include "random.h"

static const uint8_t GF4_ADDITION[4][4] = {
        {0, 1, 2, 3},
        {1, 0, 3, 2},
        {2, 3, 0, 1},
        {3, 2, 1, 0}
};

static const uint8_t GF4_MULTIPLICATION[4][4] = {
        {0, 0, 0, 0},
        {0, 1, 2, 3},
        {0, 2, 3, 1},
        {0, 3, 1, 2}
};

static const uint8_t GF4_DIVISION[4][3] ={
        {0, 0, 0},
        {1, 3, 2},
        {2, 1, 3},
        {3, 2, 1}
};

/**
 * @brief A class representing GF(4).
 *
 * GF(4) is a finite field of four elements constructed as follows:
 * GF(4) = GF(2)[X] / (X^2 + X + 1), i. e. the field of remainders of all binary polynomials divided by X^2 + X + 1
 * Suppose the polynomial X^2 + X + 1 has a root alpha. Then the elements of GF(4) are:
 * GF(4) = {0, 1, alpha, alpha + 1}
 * In this class, these are represented by a uint8_t.
 * The following conversion table is used:
 *  + 0 --> ZERO
 *  + 1 --> ONE
 *  + 2 --> ALPHA
 *  + 3 --> ALPHA_PLUS_ONE
 *  Operations are precomputed as the above Cayley tables (GF4_ADDITION, GF4_MULTIPLICATION, GF4_DIVISION).
 *  Please note that in GF(4), the addition is the same operation as subtraction.
 */
class GF4 {
public:

    /**
     * @brief The default constructor for GF4, creates a zero element.
     */
    GF4() {
        value = 0;
    }

    /**
     * @brief A conversion constructor from integer to a GF4 element.
     *
     * The following conversion table is used:
     *  + 0 --> ZERO
     *  + 1 --> ONE
     *  + 2 --> ALPHA
     *  + 3 --> ALPHA_PLUS_ONE
     *
     * @throws IncorrectValueRange if the given integer is above 3
     * @param value integer value to convert, will be downcast to uint8_t
     */
    explicit GF4(size_t value) {
        if (value > 3) {
            throw IncorrectValueRange{};
        }
        this->value = value;
    }

    /**
     * @brief A copy constructor.
     *
     * @param other GF4 element to copy
     */
    GF4(const GF4& other) = default;

    auto operator=(const GF4& other) -> GF4& = default;

    /**
     * @brief Test whether the element is zero.
     *
     * @return true if zero, false otherwise
     */
    [[nodiscard]] auto is_zero() const -> bool {
        return value == 0;
    }

    /**
     * @brief Test whether the element is one.
     *
     * @return true if one, false otherwise
     */
    [[nodiscard]] auto is_one() const -> bool {
        return value == 1;
    }

    /**
     * @brief Get a string representation of the element.
     *
     * This is mainly for printing.
     *
     * @return
     */
    [[nodiscard]] auto to_string() const -> std::string {
        switch (value) {
            case 0:
                return "0";
            case 1:
                return "1";
            case 2:
                return "alpha";
            case 3:
                return "(alpha + 1)";
        }
    }

    auto operator+(const GF4& other) const -> GF4 {
        auto val = GF4_ADDITION[value][other.value];
        return GF4{val};
    }

    auto operator-(const GF4& other) const -> GF4 {
        // in GF4, addition is the same as subtraction
        auto val = GF4_ADDITION[value][other.value];
        return GF4{val};
    }

    auto operator+=(const GF4& other) -> GF4& {
        value = GF4_ADDITION[value][other.value];
        return *this;
    }

    auto operator-=(const GF4& other) -> GF4& {
        // in GF4, addition is the same as subtraction
        value = GF4_ADDITION[value][other.value];
        return *this;
    }

    auto operator*(const GF4& other) const -> GF4 {
        uint8_t val = GF4_MULTIPLICATION[value][other.value];
        return GF4{val};
    }

    auto operator*=(const GF4& other) -> GF4& {
        value = GF4_MULTIPLICATION[value][other.value];
        return *this;
    }

    auto operator/(const GF4& other) const -> GF4 {
        if (other.value == 0) {
            throw DivisionByZero();
        }
        auto val = GF4_DIVISION[value][other.value - 1];
        return GF4{val};
    }

    auto operator/=(const GF4& other) -> GF4& {
        if (other.value == 0) {
            throw DivisionByZero{};
        }
        value = GF4_DIVISION[value][other.value - 1];
        return *this;
    }

    /**
     * @brief Generate a random vector with given hamming weight.
     *
     * The nonzero elements are drawn from GF(4) uniformly at random. The vector is shuffled using Fisher-Yates algorithm.
     *
     * @param length The length of the vector.
     * @param weight The hamming weight of the vector, i. e. the number of nonzero entries.
     * @return Randomly generated vector of a given hamming weight.
     */
    static auto random_weighted_vector(size_t length, size_t weight) -> std::vector<GF4> {
        std::vector<GF4> out;
        for (size_t i = 0; i < weight; ++i) {
            GF4 val{Random::integer<uint8_t>(1, 3)};
            out.push_back(val);
        }
        for (size_t i = weight; i < length; ++i) {
            out.emplace_back();
        }
        for (size_t i = 0; i < length; ++i) {
            size_t j = Random::integer(i, length);
            if (i != j) {
                std::iter_swap(out.begin() + i, out.begin() + j);
                // std::swap(out[i], out[j]);
            }
        }
        return out;
    }

    /**
     * @brief Generate a random vector.
     *
     * Values are drawn from GF(4) uniformly at random.
     *
     * @param length
     * @return
     */
    static auto random_vector(size_t length) -> std::vector<GF4> {
        std::vector<GF4> out;
        for (size_t i = 0; i < length; ++i) {
            GF4 val{Random::integer<uint8_t>(0, 3)};
            out.push_back(val);
        }
        return out;
    }

    /**
     * @brief Get all nonzero elements of GF(4).
     *
     * This method is necessary for symbol flipping algorithm to work.
     *
     * @return A vector of nonzero elements of GF(4).
     */
    static auto nonzero_elements() -> std::vector<GF4> {
        static GF4 nonzero_elements_array[3] = {GF4{1}, GF4{2}, GF4{3}};
        static std::vector<GF4> nonzero_elements{nonzero_elements_array, nonzero_elements_array + 3};
        return nonzero_elements;
    }

private:
    uint8_t value;
};

#endif //MDPC_GF4_CPP_GF4_H
