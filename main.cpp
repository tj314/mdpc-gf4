#include "src/gf4.h"
#include "src/contexts.h"

int main() {
    auto contexts = generate_contexts<GF4>(2339, 37); // currently hangs
    EncodingContext<GF4> ec = contexts.first;
    DecodingContext<GF4> dc = contexts.second;
    const std::vector<GF4> message = GF4::random_vector(2339);
    auto encoded = ec.encode(message);
    auto maybe_decoded = dc.decode(encoded, 100);
    if (maybe_decoded) {
        std::vector<GF4> decoded = maybe_decoded.value();
    }

    /*
    Polynomial<GF4> p1{2}; // invertible
    p1.set_coefficient(0, 1);
    p1.set_coefficient(1, 1);
    p1.set_coefficient(2, 1);

    Polynomial<GF4> p2{4}; // not invertible
    p2.set_coefficient(1, 2);
    p2.set_coefficient(4, 2);

    Polynomial<GF4> modulus{8};
    modulus.set_coefficient(0, 1);
    modulus.set_coefficient(8, 1);

    auto inv = p1.invert(modulus);

    if (inv) {
        // std::cout << "inverted!"<< std::endl;
        Polynomial<GF4> tmp = (p1 * inv.value()) % modulus;
        if (tmp.is_one()) {
            // std::cout << "inverse correct!" << std::endl;
            p1.set_coefficient(0, 0);
        } else {
            // std::cout << "inverse incorrect!" << std::endl;
            p1.set_coefficient(0, 1);
        }
    } else {
        // std::cout << "inverse not found!" << std::endl;
        p1.set_coefficient(0, 2);
    }
    */
    return 0;
}
