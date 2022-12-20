#ifndef MDPC_GF4_VECTOR_UTILS_H
#define MDPC_GF4_VECTOR_UTILS_H

template<typename T>
auto is_vector_zero(const std::vector<T>& vec) -> bool {
    for (const T& t: vec) {
        if (!t.is_zero()) {
            return false;
        }
    }
    return true;
}


template<typename T>
auto hamming_weight(const std::vector<T>& vec) -> size_t {
    size_t weight = 0;
    for (const T& t: vec) {
        if (!t.is_zero()) {
            weight += 1;
        }
    }
    return weight;
}

template<typename T>
auto sum(const std::vector<T>& v) -> T {
    T s;
    for (const T& val: v) {
        s += val;
    }
    return s;
}

#endif //MDPC_GF4_VECTOR_UTILS_H
