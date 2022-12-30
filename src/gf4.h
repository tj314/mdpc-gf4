#ifndef MDPC_GF4_GF4_H
#define MDPC_GF4_GF4_H

#include <string>
#include <vector>
#include "custom_exceptions.h"
#include "random.h"

#define GF4_MAX_VALUE 3

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
    GF4() : value(0) {}

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
        if (value > GF4_MAX_VALUE) {
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
                return "a";
            default:  // value == 3
                return "(a + 1)";
        }
    }

    auto operator+(const GF4& other) const -> GF4 {
        size_t val = value ^ other.value;
        return GF4{val};
    }

    auto operator+=(const GF4& other) -> GF4& {
        value ^= other.value;
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
     * @brief get maximum value of element in GF(4)
     *
     * In GF(4), the maximum value is 3.
     *
     * @return 3
     */
    [[nodiscard]] static auto get_max_value() -> size_t {
        return GF4_MAX_VALUE;
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

#endif //MDPC_GF4_GF4_H
