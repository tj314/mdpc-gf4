# McEliece cryptosystem over non binary QC-MDPC codes

This repository contains an implementation of McEliece cryptosystem over non binary QC-MDPC codes. This idea was presented in the article [Using Non-Binary LDPC and MDPC Codes in the McEliece Cryptosystem](https://www.researchgate.net/publication/337229244_Using_Non-Binary_LDPC_and_MDPC_Codes_in_the_McEliece_Cryptosystem).

Be warned, random reader, that this implementation is:

- a work in progress

- intended only for use in research

- not secure (it does not use a CSPRNG, it is probably vulnerable to side channel attacks, etc.)

### How to run

The entire implementation consists of header-only library and an example main.cpp file. There are no dependencies. A C++17 compiler is needed for compilation (e. g. g++). You can compile and run the implementation as follows:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o main
./main
```

You may add `-g` flag to produce a binary for debugging.

Alternatively, there is a CMake file for use with IDEs, such as CLion.

### General usage

In your main.cpp, include `contexts.h` and the header file for the desired finite field. An implementation for GF(4) is provided in `gf4.h`. You may also implement your own finite field to be used. In that case, please refer to `gf4.h` and provide equivalents to all of its methods in your implementation.

`context.h` defines `EncodingContext` and `DecodingContext` classes. These classes contain public key (matrix G) and private key (matrix H) respectively. They have functions for encoding and decoding. As G and H need to be generated together, the function `generate_contexts_over_GF2N` generates these matrices and instantiates the classes `EncodingContext` and `DecodingContext`. Internally, G and H are represented as vectors, block-wise.

Call `generate_contexts_over_GF2N` with `block_size` and `block_weight` as per [Using Non-Binary LDPC and MDPC Codes in the McEliece Cryptosystem](https://www.researchgate.net/publication/337229244_Using_Non-Binary_LDPC_and_MDPC_Codes_in_the_McEliece_Cryptosystem). For instance, `block_size = 2339` and `block_weight = 37`. This parameter set is recommended for use with GF(4) and allows for encoding vectors of length 2339.

A full example of usage follows:

```cpp

auto contexts = generate_contexts_over_GF2N<GF4>(2339, 37);
EncodingContext<GF4> ec = contexts.first;
DecodingContext<GF4> dc = contexts.second;
const std::vector<GF4> message = GF4::random_vector(2339);
auto encoded = ec.encode(message);
auto maybe_decoded = dc.decode(encoded, 100);
if (maybe_decoded) {
    std::vector<GF4> decoded = maybe_decoded.value();
    // use the decoded value
    // the decoded value should be the same as message
} else {
    // decoding failed
}
```






