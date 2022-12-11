#ifndef MDPC_GF4_CPP_POLYNOMIAL_H
#define MDPC_GF4_CPP_POLYNOMIAL_H

#include <vector>
#include <string>
#include <optional>
#include "custom_exceptions.h"
#include "tuple.h"

template<typename T>
class Polynomial {
public:
    explicit Polynomial(size_t expected_degree=0) {
        if (expected_degree > 0){
            coefficients.resize(expected_degree + 1);
        }
        degree = 0;
    }

    Polynomial(const Polynomial<T>& other) = default;

    explicit Polynomial(const std::vector<T>& coeffs) {
        degree = 0;
        for (size_t i = 0; i < coeffs.size(); ++i) {
            coefficients.push_back(coeffs[i]);
            if (!coeffs[i].is_zero()) {
                degree = i;
            }
        }
    }

    [[nodiscard]] auto get_coefficient(size_t deg) const -> const T& {
        if (deg > degree) {
            return Polynomial<T>::zero;
        } else {
            return coefficients[deg];
        }
    }

    auto set_coefficient(size_t deg, T value) -> void {
        if (deg >= coefficients.size()) {
            coefficients.resize(deg + 1);
        }
        if (value.is_zero() && deg == degree) {
            coefficients[deg] = value;
            for (size_t i = degree; i > 0; --i) {
                if (!coefficients[i].is_zero()) {
                    degree = i;
                    return;
                }
            }
            degree = 0;
        } else if (deg > degree) {
            coefficients[deg] = value;
            degree = deg;
        } else {
            coefficients[deg] = value;
        }
    }

    auto set_coefficient(size_t deg, size_t value) -> void {
        auto val = T{value};
        set_coefficient(deg, val);
    }

    auto get_degree() -> size_t {
        return degree;
    }

    auto to_vector() -> std::vector<T> {
        return coefficients;
    }

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

    [[nodiscard]] auto is_zero() const -> bool {
        return (coefficients.empty()) || (degree == 0 && coefficients[0].is_zero());
    }

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

    auto invert(const Polynomial<T>& modulus) const -> std::optional<Polynomial<T>> {
        /*
        Polynomial<T> t{modulus.degree};
        Polynomial<T> r{modulus};
        Polynomial<T> newt{modulus.degree};
        newt.coefficients[0] = T{1};
        Polynomial<T> newr{*this};

        while (!newr.is_zero()) {
            auto res = r.div_rem(newr);
            Polynomial<T> tmp{r};
            r = newr;
            newr = tmp - res.second;
            tmp = t;
            t = newt;
            newt = tmp - (res.first * newt);
        }

        if (r.degree > 0) {
            return {};
        } else {
            Polynomial<T> result{modulus.degree};
            result.coefficients[0] = T{1};
            result /= r;
            result *= t;
            return result;
        }
        */

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
            // std::cout << a.to_string() << " | " << b.to_string() << " | " << q.to_string() << " | " << s.to_string() << " | " << t.to_string() << std::endl;
            while (!q.is_zero()) {
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
                // std::cout << a.to_string() << " | " << b.to_string() << " | " << q.to_string() << " | " << s.to_string() << " | " << t.to_string() << std::endl;
                if (b.degree == 0 && b.coefficients[0].is_one()) {
                    return t;
                }
            }
            return {};
        }
    }

private:
    std::vector<T> coefficients;
    size_t degree;
    inline static const T zero{0};
};
#endif //MDPC_GF4_CPP_POLYNOMIAL_H
