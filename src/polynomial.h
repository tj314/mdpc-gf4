#ifndef MDPC_GF4_POLYNOMIAL_H
#define MDPC_GF4_POLYNOMIAL_H

#include <vector>
#include <string>
#include <optional>
#include <tuple>
#include "custom_exceptions.h"
#include "xgcd.h"


/**
 * @brief An implementation of polynomial over a GF(2^N) field.
 *
 * This class provides polynomial operations for polynomials over finite fields GF(2^N) for some natural number N.
 *
 * @tparam T Finite field to be used, it is expected to define all methods that GF4 defines and be of type GF(2^N).
 */
template<typename T>
class PolynomialGF2N {
public:

    /**
     * Construct an empty polynomial.
     */
    PolynomialGF2N() {
        coefficients.push_back(T{0});
    };

    /**
     * @brief A copy constructor.
     *
     * @param other A polynomial to copy.
     */
    PolynomialGF2N(const PolynomialGF2N<T>& other) = default;
    auto operator=(const PolynomialGF2N<T>& other) -> PolynomialGF2N<T>& = default;

    /**
     * @brief Construct a polynomial from a vector of coefficients and set its degree.
     *
     * @param coeffs
     */
    explicit PolynomialGF2N(const std::vector<T>& coeffs) {
        if (coeffs.empty()) {
            coefficients.push_back(T{0});
        } else {
            size_t degree = 0;
            for (size_t i = 0; i < coeffs.size(); ++i) {
                if (!coeffs[i].is_zero()) {
                    degree = i;
                }
            }
            if (degree != 0) {
                coefficients = coeffs;
                coefficients.resize(degree + 1);
            } else {
                coefficients = {};
                coefficients.push_back(T{0});
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
        if (deg > get_degree()) {
            return PolynomialGF2N<T>::zero;
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
        if (deg > get_degree() && !value.is_zero()) {
            coefficients.resize(deg + 1);
            coefficients[deg] = value;
        } else if (deg == get_degree() && value.is_zero()) {
            coefficients[deg] = value;
            size_t degree = 0;
            for (size_t i = get_degree(); i > 0; --i) {
                if (!coefficients[i].is_zero()) {
                    degree = i;
                    break;
                }
            }
            if (degree == 0) {
                coefficients = {};
            } else {
                coefficients.resize(degree + 1);
            }
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
        return coefficients.size() - 1;
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
            for (size_t deg = 0; deg <= get_degree(); ++deg) {
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
        return (coefficients.empty()) || (get_degree() == 0 && coefficients[0].is_zero());
    }

    /**
     * @brief Test whether the polynomial is one.
     *
     * @return true if one, false otherwise
     */
    [[nodiscard]] auto is_one() const -> bool {
        return (coefficients.empty()) || (get_degree() == 0 && coefficients[0].is_one());
    }

    auto operator+(const PolynomialGF2N<T>& other) const -> PolynomialGF2N<T> {
        if (get_degree() > other.get_degree()) { // DO NOT change to >=
            PolynomialGF2N<T> out;
            out.coefficients.resize(get_degree() + 1);
            for (size_t i = 0; i <= get_degree(); ++i) {
                out.coefficients[i] = coefficients[i] + other.get_coefficient(i);
            }
            return out;
        } else {
            PolynomialGF2N<T> out;
            out.coefficients.resize(other.get_degree() + 1);
            size_t degree = 0;
            for (size_t i = 0; i <= other.get_degree(); ++i) {
                out.coefficients[i] = get_coefficient(i) + other.coefficients[i];
                if (!out.coefficients[i].is_zero()) {
                    degree = i;
                }
            }
            out.coefficients.resize(degree + 1);
            return out;
        }
    }

    auto operator+=(const PolynomialGF2N<T>& other) -> PolynomialGF2N<T>& {
        if (get_degree() > other.get_degree()) { // DO NOT change to >=
            for (size_t i = 0; i <= other.get_degree(); ++i) {
                coefficients[i] += other.coefficients[i];
            }
        } else {
            coefficients.resize(other.get_degree() + 1);
            size_t degree = 0;
            for (size_t i = 0; i <= other.get_degree(); ++i) {
                coefficients[i] += other.coefficients[i];
                if (!coefficients[i].is_zero()) {
                    degree = i;
                }
            }
            coefficients.resize(degree + 1);
        }
        return *this;
    }

    auto operator-(const PolynomialGF2N<T>& other) const -> PolynomialGF2N<T> {
        return *this + other;
    }

    auto operator-=(const PolynomialGF2N<T>& other) -> PolynomialGF2N<T>& {
        *this += other;
        return *this;
    }

    auto operator*(const PolynomialGF2N<T>& other) const -> PolynomialGF2N<T> {
        PolynomialGF2N<T> out;
        out.coefficients.resize(get_degree() + other.get_degree() + 1);
        size_t degree = 0;
        for (size_t i = 0; i <= get_degree(); ++i) {
            for (size_t j = 0; j <= other.get_degree(); ++j) {
                out.coefficients[i+j] += (coefficients[i] * other.coefficients[j]);
                if (!out.coefficients[i+j].is_zero() && (i+j) > degree) {
                    degree = i + j;
                }
            }
        }
        out.coefficients.resize(degree + 1);
        return out;
    }

    auto operator*=(const PolynomialGF2N<T>& other) -> PolynomialGF2N<T>& {
        std::vector<T> new_coefficients;
        new_coefficients.resize(get_degree() + other.get_degree() + 1);
        size_t degree = 0;
        for (size_t i = 0; i <= get_degree(); ++i) {
            for (size_t j = 0; j <= other.degree; ++j) {
                new_coefficients[i+j] += (coefficients[i] * other.coefficients[j]);
                if (!coefficients[i+j].is_zero() && (i+j) > degree) {
                    degree = i + j;
                }
            }
        }
        new_coefficients.resize(degree + 1);
        coefficients = new_coefficients;
        return *this;
    }

    auto operator*(const T& scalar) const -> PolynomialGF2N<T> {
        PolynomialGF2N<T> out{*this};
        size_t max_deg = 0;
        for (size_t deg = 0; deg <= out.degree; ++deg) {
            out.coefficients[deg] *= scalar;
            if (!out.coefficients[deg].is_zero()) {
                max_deg = deg;
            }
        }
        out.coefficients.resize(max_deg + 1);
        return out;
    }

    auto operator*=(const T& scalar) -> PolynomialGF2N<T>& {
        size_t max_deg = 0;
        for (size_t deg = 0; deg <= get_degree(); ++deg) {
            coefficients[deg] *= scalar;
            if (!coefficients[deg].is_zero()) {
                max_deg = deg;
            }
        }
        coefficients.resize(max_deg + 1);
        return *this;
    }

    /**
     * @brief Divide and calculate the remainder at the same time.
     *
     * This is more efficient than calling / and % operators separately,
     * as these operators will internally call this function and discard the unwanted part of the result.
     *
     * @param other PolynomialGF2N to divide by.
     * @return A tuple containing the result of division and the remainder.
     */
    auto div_rem(const PolynomialGF2N<T>& other) const -> std::tuple<PolynomialGF2N<T>, PolynomialGF2N<T>> {
        if (other.is_zero()) {
            throw DivisionByZero{};
        }
        if (get_degree() < other.get_degree()) {
            PolynomialGF2N<T> q;
            PolynomialGF2N<T> r{*this};
            return std::make_tuple(q, r);
        } else {
            PolynomialGF2N<T> q;
            PolynomialGF2N<T> r{*this};

            T other_lead = other.coefficients[other.get_degree()];

            while (!r.is_zero() && r.get_degree() >= other.get_degree()) {
                PolynomialGF2N<T> t;
                size_t t_degree = r.get_degree() - other.get_degree();
                t.coefficients.resize(t_degree + 1);
                t.coefficients[t_degree] = r.coefficients[r.get_degree()] / other_lead;
                q += t;
                r -= (t * other);
            }
            return std::make_tuple(q, r);
        }
    }

    /**
     * Divide the polynomial by x^deg.
     *
     * This is sometimes useful and is much faster than Polynomial::div_rem().
     * Division of the polynomial by x^k for some integer k >= 0 can by calculated by simply
     * removing the first k entries from the coefficients vector, i.e. shifting the vector left by k positions.
     *
     * @param deg
     * @return
     */
    auto div_x_to_deg(size_t deg) -> PolynomialGF2N<T> {
        PolynomialGF2N<T> out{*this};
        out.coefficients.erase(out.coefficients.begin(), out.coefficients.begin() + deg);
        return out;
    }

    auto operator/(const PolynomialGF2N<T>& other) const -> PolynomialGF2N<T> {
        return std::get<0>(div_rem(other));
    }

    auto operator/=(const PolynomialGF2N<T>& other) -> PolynomialGF2N<T>& {
        *this = std::get<0>(div_rem(other));
        return *this;
    }

    auto operator%(const PolynomialGF2N<T>& other) const -> PolynomialGF2N<T> {
        return std::get<1>(div_rem(other));
    }

    auto operator%=(const PolynomialGF2N<T>& other) -> PolynomialGF2N<T> {
        *this = std::get<1>(div_rem(other));
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
    auto invert_slow(const PolynomialGF2N<T>& modulus) const -> std::optional<PolynomialGF2N<T>> {
        if (is_zero()) {
            return {};
        } else if (modulus.is_zero()) {
            throw DivisionByZero{};
        }

        PolynomialGF2N<T> a{*this};
        PolynomialGF2N<T> b{modulus};
        PolynomialGF2N<T> t = PolynomialGF2N<T>::make_one();
        PolynomialGF2N<T> s = PolynomialGF2N<T>::make_zero();
        while(!b.is_zero()) {
            auto [d, r] = a.div_rem(b);
            std::tie(a, b) = std::make_tuple(b, r);
            std::tie(t, s) = std::make_tuple(s, t + d * s);
        }
        if(a.get_degree() > 0) {
            return {};
        } else {
            return t / a;
        }
    }

    /**
     * @brief Calculate the multiplicative inverse of the polynomial mod the provided modulus.
     *
     * This implements extended euclidean algorithm.
     *
     * @param modulus A polynomial.
     * @return Multiplicative inverse of the polynomial if it exists, nothing otherwise.
     */
    auto invert(const PolynomialGF2N<T>& modulus) const -> std::optional<PolynomialGF2N<T>> {
        if (modulus.is_zero()) {
            throw DivisionByZero{};
        }
        PolynomialGF2N<T> a = modulus;
        PolynomialGF2N<T> b = *this % modulus;
        auto tr = std::get<1>(full_gcd(a, b));
        auto g = tr.a11 * a - tr.a01 * b;
        if(g.get_degree() != 0) {
            return {};
        } else {
            return tr.a01 / g;
        }
    }

    static auto make_zero() -> PolynomialGF2N<T> {
        return PolynomialGF2N<T>{};
    }

    static auto make_one() -> PolynomialGF2N<T> {
        PolynomialGF2N<T> out;
        out.coefficients.resize(1);
        out.coefficients[0] = T{1};
        return out;
    }

private:
    std::vector<T> coefficients;
    inline static const T zero{0};
};

#endif //MDPC_GF4_POLYNOMIAL_H
