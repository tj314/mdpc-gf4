#ifndef MDPC_GF4_ENCODING_CONTEXT_H
#define MDPC_GF4_ENCODING_CONTEXT_H

#include <vector>
#include <optional>
#include "polynomial.h"
#include "custom_exceptions.h"
#include "vector_utils.h"

/**
 * @brief Class that hold the public key G and provides encoding functionality.
 *
 * The public key G is a matrix, but here it is represented as a vector.
 * This vector corresponds to the second block of G (but not transposed as it is unnecessary for encoding).
 * The first block is an identity matrix and it therefore unnecessary to store it.
 *
 * @tparam T Finite field to be used.
 */
template <typename T>
class EncodingContext {
public:
    EncodingContext() = default;
    EncodingContext(const std::vector<T>& second_block_G, size_t block_size) : second_block_G(second_block_G), block_size(block_size) {}

    /**
     * @brief Encode a message.
     *
     * The message must be of length block_size.
     * The encoded message is calculated as mG.
     *
     * @param message A vector of length block_size.
     * @return Encoded message stored in a vector of length 2*block_size.
     */
    auto encode(const std::vector<T>& message) -> std::vector<T> {
        if (message.size() != block_size) {
            throw IncorrectInputVectorLength{};
        }
        std::vector<T> encoded;
        encoded.reserve(2*block_size);
        for (unsigned i = 0; i < block_size; ++i) {
            encoded.push_back(message[i]);
        }

        for (unsigned i = block_size; i > 0; --i) {
            T tmp{};
            for (unsigned j = 0; j < block_size; ++j) {
                tmp += (message[j] * second_block_G[(i + j) % block_size]);
            }
            encoded.push_back(tmp);
        }
        return encoded;
    }
private:
    std::vector<T> second_block_G;
    size_t block_size;
};

/**
 * @brief Class that holds the private key H and provides decoding functionality.
 *
 * The private key H is a matrix, but here it is stored as two vectors h0 and h1
 * corresponding to the first rows of the blocks of H.
 *
 * @tparam T Finite field to be used.
 */
template <typename T>
class DecodingContext {
public:
    DecodingContext() = default;

    DecodingContext(const std::vector<T> &h0, const std::vector<T> &h1, size_t block_size, size_t block_weight) : h0(h0), h1(h1), block_size(block_size), block_weight(block_weight) {}

    /**
     * @brief Calculate the syndrome of a given vector.
     *
     * The vector is expected to be of length 2*block_size.
     *
     * @param vec Avector of length 2*block_size.
     * @return Syndrome stored in a vector of length block_size.
     */
    auto calculate_syndrome(const std::vector<T>& vec) -> std::vector<T> {
        std::vector<T> syndrome;
        for (unsigned i = block_size; i > 0; --i) {
            T tmp{};
            for (unsigned j = 0; j < block_size; ++j) {
                tmp += (h0[(i + j) % block_size] * vec[j]);
                tmp += (h1[(i + j) % block_size] * vec[block_size + j]);
            }
            syndrome.push_back(tmp);
        }
        return syndrome;
    }

    /**
     * @brief Decode the given vector.
     *
     * Decoding tries to find the used error vector. Message to decode must be of length 2*block_size.
     * There is a nonzero probability that the decoding will fail.
     *
     * @param message A vector of length 2*block_size.
     * @param num_iterations Number of iterations of decoding.
     * @return The error vector of length 2*block_size on success, nothing on failure.
     */
    auto decode(const std::vector<T>& message, size_t num_iterations) -> std::optional<std::vector<T>> {
        if (message.size() != 2*block_size) {
            throw IncorrectInputVectorLength{};
        }

        std::vector<T> syndrome = calculate_syndrome(message);
        size_t syndrome_weight = hamming_weight(syndrome);
        std::vector<T> error_vector;
        error_vector.resize(2 * block_size);

        if (syndrome_weight == 0) {
            return error_vector;
        }

        std::vector<T> nonzero_values = T::nonzero_elements();

        for (size_t iter = 0; iter < num_iterations; ++iter) {
            size_t sigma_max = 2*block_size + 1;
            T a_max{};
            size_t pos = 2*block_size + 1;
            for (size_t j = 0; j < 2*block_size; ++j) {
                auto& h_block = (j < block_size) ? h0: h1;
                for (const T& a: nonzero_values) {
                    // calculate the hamming weight of s - a*h_j
                    size_t w = 0;
                    for (size_t i = 0; i < syndrome.size(); ++i) {
                        T tmp = syndrome[i] - (a * h_block[i]);
                        w += (size_t)tmp.is_zero();
                    }
                    size_t sigma = syndrome_weight - w;
                    if (sigma > sigma_max) {
                        sigma_max = sigma;
                        a_max = a;
                        pos = j;
                    }
                }
            }
            auto& h_block = (pos < block_size) ? h0: h1;
            syndrome_weight = 0;
            for (size_t i = 0; i < syndrome.size(); ++i) {
                syndrome[i] -= (a_max*h_block[i]);
                syndrome_weight += (size_t)syndrome[i].is_zero();
            }
            error_vector[pos] = a_max;
        }
        if (is_vector_zero(syndrome)) {
            return error_vector;
        } else {
            return {};
        }
    }
private:
    std::vector<T> h0;
    std::vector<T> h1;
    size_t block_size;
    size_t block_weight;
};

/**
 * @brief Generate public (matrix G) and private (matrix H) keys and the classes that store them.
 *
 * @tparam T Finite field to be used.
 * @param block_size The size of the circulant block of the matrices.
 * @param block_weight The hamming weight of the row of the block of the matrix H.
 * @return Instantiated classes EncodingContext and DecodingContext.
 */
template<typename T>
auto generate_contexts(size_t block_size, size_t block_weight) -> DistinctTuple<EncodingContext<T>, DecodingContext<T>> {
    std::vector<T> h0 = Random::random_weighted_vector_over_GF2N<GF4>(block_size, block_weight);
    Polynomial<T> h0_poly{h0};

    Polynomial<T> modulus{block_size + 1};
    modulus.set_coefficient(0, T{1});
    modulus.set_coefficient(block_size, T{1});

    std::vector<T> h1;
    while (true) {
        h1 = Random::random_weighted_vector_over_GF2N<GF4>(block_size, block_weight);
        while (!sum(h1).is_zero()) {
            h1 = Random::random_weighted_vector_over_GF2N<GF4>(block_size, block_weight);
        }
        Polynomial<T> h1_poly{h1};
        auto inverse = h1_poly.invert(modulus);
        if (inverse) {
            Polynomial<T> zero_poly{0};
            Polynomial<T> second_block_G_poly = zero_poly - (h0_poly * inverse.value());
            T minus_one = T{} - T{1}; // TODO: this is somewhat of a hacky way to generate -1, consider introducing a method in galois field
            second_block_G_poly *= minus_one;
            second_block_G_poly %= modulus;
            EncodingContext<T> ec{second_block_G_poly.to_vector(), block_size};
            DecodingContext<T> dc{h0, h1, block_size, block_weight};
            return DistinctTuple{ec, dc};
        }
    }

}

#endif //MDPC_GF4_ENCODING_CONTEXT_H
