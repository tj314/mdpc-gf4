#ifndef MDPC_GF4_CPP_CUSTOM_EXCEPTIONS_H
#define MDPC_GF4_CPP_CUSTOM_EXCEPTIONS_H

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

#endif //MDPC_GF4_CPP_CUSTOM_EXCEPTIONS_H
