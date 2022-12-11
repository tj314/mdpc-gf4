#ifndef MDPC_GF4_CPP_TUPLE_H
#define MDPC_GF4_CPP_TUPLE_H

template<typename T, typename R>
struct DistinctTuple {
    DistinctTuple() = default;
    DistinctTuple(T first, R second): first(first), second(second) {}
    T first;
    R second;
};

template<typename T>
using Tuple = DistinctTuple<T, T>;

#endif //MDPC_GF4_CPP_TUPLE_H
