#include "src/gf4.h"
#include "src/contexts.h"

#include <iostream>

int main() {
    /*
    auto [ec, dc] = generate_contexts_over_GF2N<GF4>(2339, 37);
    const std::vector<GF4> message = Random::random_vector_over_GF2N<GF4>(2339);
    auto encoded = ec.encode(message);
    auto maybe_decoded = dc.decode(encoded, 100);
    if (maybe_decoded) {
        std::vector<GF4> decoded = maybe_decoded.value();
    }
    */
    PolynomialGF2N<GF4> p1; // invertible
    p1.set_coefficient(0, 1);
    p1.set_coefficient(1, 1);
    p1.set_coefficient(2, 1);

    PolynomialGF2N<GF4> p2; // not invertible
    p2.set_coefficient(1, 2);
    p2.set_coefficient(4, 2);

    PolynomialGF2N<GF4> modulus;
    modulus.set_coefficient(0, 1);
    modulus.set_coefficient(8, 1);

    auto& p = p2; // select polynomial to test

    auto inv = p.invert(modulus);
    if (inv) {
        std::cout << "inverted!"<< std::endl;
        PolynomialGF2N<GF4> tmp = (p * inv.value()) % modulus;
        if (tmp.is_one()) {
            std::cout << "inverse correct!" << std::endl;
            p1.set_coefficient(0, 0);
        } else {
            std::cout << "inverse incorrect!" << std::endl;
            p1.set_coefficient(0, 1);
        }
    } else {
        std::cout << "inverse not found!" << std::endl;
        p1.set_coefficient(0, 2);
    }

    return 0;
}
