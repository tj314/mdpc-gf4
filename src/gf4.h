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
     * @param other GF4 element to copy
     */
    GF4(const GF4& other) = default;

    [[nodiscard]] auto is_zero() const -> bool {
        return value == 0;
    }

    [[nodiscard]] auto is_one() const -> bool {
        return value == 1;
    }

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
                std::swap(out[i], out[j]);
            }
        }
        return out;
    }

    static auto random_vector(size_t length) -> std::vector<GF4> {
        std::vector<GF4> out;
        for (size_t i = 0; i < length; ++i) {
            GF4 val{Random::integer<uint8_t>(0, 3)};
            out.push_back(val);
        }
        return out;
    }

    static auto nonzero_elements() -> std::vector<GF4> {
        static GF4 nonzero_elements_array[3] = {GF4{1}, GF4{2}, GF4{3}};
        static std::vector<GF4> nonzero_elements{nonzero_elements_array, nonzero_elements_array + 3};
        return nonzero_elements;
    }

private:
    uint8_t value;
};




#endif //MDPC_GF4_CPP_GF4_H
