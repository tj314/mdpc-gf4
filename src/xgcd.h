// This implementation is heavily based on https://codeforces.com/blog/entry/101850
// The site was accessed on the 1st of january 2023.

#ifndef MDPC_GF4_XGCD_H
#define MDPC_GF4_XGCD_H

#include <array>
#include "polynomial.h"
#include "custom_exceptions.h"

template<typename T>
class PolynomialGF2N; // forward declaration

template<typename T>
struct TransformMatrixGF2N {
    PolynomialGF2N<T> a00, a01, a10, a11;
    TransformMatrixGF2N() = default;
    TransformMatrixGF2N(const PolynomialGF2N<T> &a00, const PolynomialGF2N<T> &a01, const PolynomialGF2N<T> &a10,
                        const PolynomialGF2N<T> &a11) : a00(a00), a01(a01), a10(a10), a11(a11) {}

    auto operator*(const TransformMatrixGF2N<T>& other) -> TransformMatrixGF2N<T> {
        return TransformMatrixGF2N<T>{
            a00*other.a00 + a01*other.a10,
            a00*other.a01 + a01*other.a11,
            a10*other.a00 + a11*other.a10,
            a10*other.a01 + a11*other.a11,
        };
    }

    auto adjugate() -> TransformMatrixGF2N<T> {
        return TransformMatrixGF2N<T>{
            a11, a01, a10, a11,
        };
    }

    auto transform(PolynomialGF2N<T> a, PolynomialGF2N<T> b) -> std::tuple<PolynomialGF2N<T>, PolynomialGF2N<T>>{
        return std::make_tuple(a00*a + a01*b, a10*a + a11*b);
    }
};

template<typename T>
auto half_gcd(PolynomialGF2N<T> A, PolynomialGF2N<T> B) -> std::tuple<std::vector<PolynomialGF2N<T>>, TransformMatrixGF2N<T>> {
    if (A.get_degree() < B.get_degree()) {
        throw IncorrectPolynomialDegree{};
    }
    size_t m = (A.get_degree() + 1) / 2;
    if(B.get_degree() < m) {
        return std::make_tuple(
            std::vector<PolynomialGF2N<T>>{},
            TransformMatrixGF2N<T>{
                PolynomialGF2N<T>::make_one(),
                PolynomialGF2N<T>::make_zero(),
                PolynomialGF2N<T>::make_zero(),
                PolynomialGF2N<T>::make_one()
            }
        );
    } else {
        auto [ar, Tr] = half_gcd(A.div_x_to_deg(m), B.div_x_to_deg(m));
        std::tie(A, B) = Tr.adjugate().transform(A, B);
        if(B.get_degree() < m) {
            return {ar, Tr};
        } else {
            auto [ai, R] = A.div_rem(B);
            std::tie(A, B) = std::make_pair(B, R);
            size_t k = 2 * m - B.get_degree();
            auto [as, Ts] = half_gcd(A.div_x_to_deg(k), B.div_x_to_deg(k));
            ar.push_back(ai);
            ar.insert(ar.end(), as.begin(), as.end());
            return {ar, Tr * TransformMatrixGF2N<T>{ai, PolynomialGF2N<T>::make_one(), PolynomialGF2N<T>::make_one(), PolynomialGF2N<T>::make_zero()} * Ts};
        }
    }
}

template<typename T>
auto full_gcd(PolynomialGF2N<T> a, PolynomialGF2N<T> b) -> std::tuple<std::vector<PolynomialGF2N<T>>, TransformMatrixGF2N<T>> {
    std::vector<PolynomialGF2N<T>> ak;
    std::vector<TransformMatrixGF2N<T>> trs;
    while(!b.is_zero()) {
        if(2 * b.get_degree() > a.get_degree()) {
            auto [tmp, tr] = half_gcd(a, b);
            ak.insert(ak.end(), tmp.begin(), tmp.end());
            trs.push_back(tr);
            std::tie(a, b) = trs.back().adjugate().transform(a, b);
        } else {
            auto [d, r] = a.div_rem(b);
            ak.push_back(d);
            trs.push_back(TransformMatrixGF2N<T>{d, PolynomialGF2N<T>::make_one(), PolynomialGF2N<T>::make_one(), PolynomialGF2N<T>::make_zero()});
            std::tie(a, b) = std::make_tuple(b, r);
        }
    }
    trs.emplace_back(TransformMatrixGF2N<T>{
        PolynomialGF2N<T>::make_one(),
        PolynomialGF2N<T>::make_zero(),
        PolynomialGF2N<T>::make_zero(),
        PolynomialGF2N<T>::make_one()
    });
    while(trs.size() >= 2) {
        trs[trs.size() - 2] = trs[trs.size() - 2] * trs[trs.size() - 1];
        trs.pop_back();
    }
    return {ak, trs.back()};
}

#endif //MDPC_GF4_XGCD_H