#include "hash_functions.h"

size_t polynomial_rolling_hash::operator() (std::string const & str) const {
    size_t hash = 0;
    size_t p = 1;
    size_t b = 19; // Base for polynomial rolling hash
    size_t mod = 3298534883309ul; // A large prime number for modulo operation
    for (char c : str) {
        hash += static_cast<size_t>(c) * p;
        p = (p * b) % mod;
    }
    return hash;

}

size_t fnv1a_hash::operator() (std::string const & str) const {
    size_t hash = 0xCBF29CE484222325;
    size_t prime = 0x00000100000001B3;
    for (char c : str) {
        hash ^= static_cast<size_t>(c);
        hash *= prime;
    }
    return hash;
}
