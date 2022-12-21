#ifndef MDPC_GF4_CUSTOM_EXCEPTIONS_H
#define MDPC_GF4_CUSTOM_EXCEPTIONS_H

#include <exception>

struct DivisionByZero : public std::exception {
    auto what() -> const char * {
        return "Division by zero!";
    }
};

struct IncorrectInputVectorLength : public std::exception {
    auto what() -> const char * {
        return "The provided input vector is of incorrect length!";
    }
};

struct IncorrectValueRange : public std::exception {
    auto what() -> const char * {
        return "The given value is out of range for this finite field!";
    }
};

struct ImpossibleHammingWeight : public std::exception {
    auto what() -> const char * {
        return "Cannot construct a vector with the request weight. The weight is more than the length!";
    }
};

struct WTF : public std::exception {
    auto what() -> const char * {
        return "This shouldn't have happened. This is a bug! read the comments!";
    }
};

#endif //MDPC_GF4_CUSTOM_EXCEPTIONS_H
