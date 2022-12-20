#ifndef MDPC_GF4_POLYNOMIAL_H
#define MDPC_GF4_POLYNOMIAL_H

#include <vector>
#include <string>
#include <optional>
#include "custom_exceptions.h"
#include "tuple.h"

/**
 * @brief An implementation of polynomial.
 *
 * This class provides polynomial operations for polynomials over finite fields.
 *
 * @tparam T Finite field to be used, it is expected to define all methods that GF4 defines.
 */
template<typename T>
class Polynomial {
public:

    /**
     * @brief Construct a polynomial with pre-allocated space.
     *
     * Constructs a polynomial with the coefficients vector of length expected degree+1.
     * The degree of the constructed polynomial will be 0.
     *
     * @param expected_degree The expected degree of created polynomial, i.e. the amount of pre-allocated space.
     */
    explicit Polynomial(size_t expected_degree=0) {
        coefficients.resize(expected_degree + 1);
        degree = 0;
    }

    /**
     * @brief A copy constructor.
     *
     * @param other A polynomial to copy.
     */
    Polynomial(const Polynomial<T>& other) = default;
    auto operator=(const Polynomial<T>& other) -> Polynomial<T>& = default;

    /**
     * @brief Construct a polynomial from a vector of coefficients and set its degree.
     *
     * @param coeffs
     */
    explicit Polynomial(const std::vector<T>& coeffs) {
        degree = 0;
        coefficients = coeffs;
        for (size_t i = 0; i < coeffs.size(); ++i) {
            if (!coeffs[i].is_zero()) {
                degree = i;
            }
        }
    }

    /**
     * @brief Get the coefficient of the given power of x in the polynomial.
     *
     * If the requested power (deg) is above the degree of the polynomial, zero is returned.
     *
     * @param deg The power of x.
     * @return The coefficient corresponding to the requested power of x.
     */
    [[nodiscard]] auto get_coefficient(size_t deg) const -> const T& {
        if (deg > degree) {
            return Polynomial<T>::zero;
        } else {
            return coefficients[deg];
        }
    }

    /**
     * @brief Set the coefficient of the given power of x to the provided value.
     *
     * Allocates more space if necessary, will work correctly even if deg > degree.
     *
     * @param deg The power of x.
     * @param value The value to set the coefficient to.
     */
    auto set_coefficient(size_t deg, T value) -> void {
        if (deg > degree && !value.is_zero()) {
            coefficients.resize(deg + 1);
            coefficients[deg] = value;
            degree = deg;
        } else if (deg == degree && value.is_zero()) {
            coefficients[deg] = value;
            for (size_t i = degree; i > 0; --i) {
                if (!coefficients[i].is_zero()) {
                    degree = i;
                    return;
                }
            }
            degree = 0;
        } else {
            coefficients[deg] = value;
        }
    }

    /**
     * @brief Set the coefficient of the given power of x to the provided value.
     *
     * Allocates more space if necessary, will work correctly even if deg > degree.
     *
     * @param deg The power of x.
     * @param value The value to set the coefficient to.
     */
    auto set_coefficient(size_t deg, size_t value) -> void {
        auto val = T{value};
        set_coefficient(deg, val);
    }

    /**
     * @brief get the degree of the polynomial.
     *
     * @return The degree of the polynomial.
     */
    [[nodiscard]] auto get_degree() const -> size_t {
        return degree;
    }

    /**
     * @brief Get a copy of the coefficients.
     *
     * @return A vector of coefficients of the polynomial.
     */
    [[nodiscard]] auto to_vector() const -> std::vector<T> {
        return coefficients;
    }

    /**
     * @brief Get a string representation of the polynomial.
     *
     * This is mainly for printing.
     *
     * @return A string representation of the polynomial.
     */
    [[nodiscard]] auto to_string() const -> std::string {
        if (is_zero()) {
            return "0";
        } else {
            std::string repr;
            std::string sep;
            for (size_t deg = 0; deg <= degree; ++deg) {
                if (!coefficients[deg].is_zero()) {
                    repr += (sep + coefficients[deg].to_string() + "*x^" + std::to_string(deg));
                    sep = " + ";
                }
            }
            return repr;
        }
    }

    /**
     * @brief Test whether the polynomial is zero.
     *
     * @return true if one, false otherwise
     */
    [[nodiscard]] auto is_zero() const -> bool {
        return (coefficients.empty()) || (degree == 0 && coefficients[0].is_zero());
    }

    /**
     * @brief Test whether the polynomial is one.
     *
     * @return true if one, false otherwise
     */
    [[nodiscard]] auto is_one() const -> bool {
        return (coefficients.empty()) || (degree == 0 && coefficients[0].is_one());
    }

    auto operator+(const Polynomial<T>& other) const -> Polynomial<T> {
        if (degree > other.degree) { // DO NOT change to >=
            Polynomial<T> out{degree};
            for (size_t i = 0; i <= degree; ++i) {
                out.coefficients[i] = coefficients[i] + other.get_coefficient(i);
            }
            out.degree = degree;
            return out;
        } else {
            Polynomial<T> out{other.degree};
            size_t deg = 0;
            for (size_t i = 0; i <= other.degree; ++i) {
                out.coefficients[i] = get_coefficient(i) + other.coefficients[i];
                if (!out.coefficients[i].is_zero()) {
                    deg = i;
                }
            }
            out.degree = deg;
            return out;
        }
    }

    auto operator+=(const Polynomial<T>& other) -> Polynomial<T>& {
        if (degree > other.degree) { // DO NOT change to >=
            for (size_t i = 0; i <= other.degree; ++i) {
                coefficients[i] += other.coefficients[i];
            }
        } else {
            coefficients.resize(other.degree + 1);
            size_t deg = 0;
            for (size_t i = 0; i <= other.degree; ++i) {
                coefficients[i] += other.coefficients[i];
                if (!coefficients[i].is_zero()) {
                    deg = i;
                }
            }
            degree = deg;
        }
        return *this;
    }

    auto operator-(const Polynomial<T>& other) const -> Polynomial<T> {
        if (degree > other.degree) { // DO NOT change to >=
            Polynomial<T> out{degree};
            for (size_t i = 0; i <= degree; ++i) {
                out.coefficients[i] = coefficients[i] - other.get_coefficient(i);
            }
            out.degree = degree;
            return out;
        } else {
            Polynomial<T> out{other.degree};
            size_t deg = 0;
            for (size_t i = 0; i <= other.degree; ++i) {
                out.coefficients[i] = get_coefficient(i) - other.coefficients[i];
                if (!out.coefficients[i].is_zero()) {
                    deg = i;
                }
            }
            out.degree = deg;
            return out;
        }
    }

    auto operator-=(const Polynomial<T>& other) -> Polynomial<T>& {
        if (degree > other.degree) { // DO NOT change to >=
            for (size_t i = 0; i <= other.degree; ++i) {
                coefficients[i] -= other.coefficients[i];
            }
        } else {
            coefficients.resize(other.degree + 1);
            size_t deg = 0;
            for (size_t i = 0; i <= other.degree; ++i) {
                coefficients[i] -= other.coefficients[i];
                if (!coefficients[i].is_zero()) {
                    deg = i;
                }
            }
            degree = deg;
        }
        return *this;
    }

    auto operator*(const Polynomial<T>& other) const -> Polynomial<T> {
        Polynomial<T> out{degree + other.degree};
        size_t deg = 0;
        for (size_t i = 0; i <= degree; ++i) {
            for (size_t j = 0; j <= other.degree; ++j) {
                out.coefficients[i+j] += (coefficients[i] * other.coefficients[j]);
                if (!out.coefficients[i+j].is_zero()) {
                    deg = i+j;
                }
            }
        }
        out.degree = deg;
        return out;
    }

    auto operator*=(const Polynomial<T>& other) -> Polynomial<T>& {
        coefficients.resize(degree + other.degree);
        size_t deg = 0;
        for (size_t i = 0; i <= degree; ++i) {
            for (size_t j = 0; j <= other.degree; ++j) {
                coefficients[i+j] += (coefficients[i] * other.coefficients[j]);
                if (!coefficients[i+j].is_zero()) {
                    deg = i+j;
                }
            }
        }
        degree = deg;
        return *this;
    }

    auto operator*(const T& scalar) const -> Polynomial<T> {
        Polynomial<T> out{*this};
        size_t max_deg = 0;
        for (size_t deg = 0; deg <= out.degree; ++deg) {
            out.coefficients[deg] *= scalar;
            if (!out.coefficients[deg].is_zero()) {
                max_deg = deg;
            }
        }
        out.degree = max_deg;
        return out;
    }

    auto operator*=(const T& scalar) -> Polynomial<T>& {
        size_t max_deg = 0;
        for (size_t deg = 0; deg <= degree; ++deg) {
            coefficients[deg] *= scalar;
            if (!coefficients[deg].is_zero()) {
                max_deg = deg;
            }
        }
        degree = max_deg;
        return *this;
    }

    /**
     * @brief Divide and calculate the remainder at the same time.
     *
     * This is more efficient than calling / and % operators separately,
     * as these operators will internally call this function and discard the unwanted part of the result.
     *
     * @param other Polynomial to divide by.
     * @return A tuple containing the result of division and the remainder.
     */
    auto div_rem(const Polynomial<T>& other) const -> Tuple<Polynomial<T>> {
        if (other.is_zero()) {
            throw DivisionByZero{};
        }
        if (degree < other.degree) {
            Polynomial<T> q;
            Polynomial<T> r{*this};
            return Tuple<Polynomial<T>>{q, r};
        } else {
            size_t dif = degree - other.degree;

            Polynomial<T> q{dif};
            Polynomial<T> r{*this};

            T other_lead = other.coefficients[other.degree];

            while (!r.is_zero() && r.degree >= other.degree) {
                Polynomial<T> t{degree};
                t.coefficients[r.degree - other.degree] = r.coefficients[r.degree] / other_lead;
                t.degree = r.degree - other.degree;
                q += t;
                r -= (t * other);
            }
            return Tuple<Polynomial<T>>{q, r};
        }
    }

    auto operator/(const Polynomial<T>& other) const -> Polynomial<T> {
        return div_rem(other).first;
    }

    auto operator/=(const Polynomial<T>& other) -> Polynomial<T>& {
        *this = div_rem(other).first;
        return *this;
    }

    auto operator%(const Polynomial<T>& other) const -> Polynomial<T> {
        return div_rem(other).second;
    }

    auto operator%=(const Polynomial<T>& other) -> Polynomial<T> {
        *this = div_rem(other).second;
        return *this;
    }

    /**
     * @brief Calculate the multiplicative inverse of the polynomial mod the provided modulus.
     *
     * This implements extended euclidean algorithm.
     *
     * @param modulus A polynomial.
     * @return Multiplicative inverse of the polynomial if it exists, nothing otherwise.
     */
    auto invert(const Polynomial<T>& modulus) const -> std::optional<Polynomial<T>> {
        if (is_zero()) {
            return {};
        } else {
            Polynomial<T> a{modulus};
            Polynomial<T> b{*this};
            auto res = a.div_rem(b);
            if (res.second.is_zero()) {
                return {};
            }
            Polynomial<T> q = res.first;
            Polynomial<T> s{modulus.degree};
            Polynomial<T> t{modulus.degree};
            t.coefficients[0] = T{1};
            while (true) {
                a = b;
                b = res.second;
                if (b.is_zero()) {
                    return {};
                }
                res = a.div_rem(b);
                Polynomial<T> news = t;
                Polynomial<T> newt = s - q*t;
                q = res.first;
                s = news;
                t = newt;
                if (b.degree == 0) {
                    return t / b;
                }
            }
        }
    }

private:
    std::vector<T> coefficients;
    size_t degree;
    inline static const T zero{0};
};
#endif //MDPC_GF4_POLYNOMIAL_H
