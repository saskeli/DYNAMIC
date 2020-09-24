#include <iostream>
#include <chrono>
#include <cstdlib>
#include "dynamic/dynamic.hpp"
#include "dynamic/internal/bufferedbv.hpp"
#include <immintrin.h>

using namespace std;
using namespace dyn;

buffered_packed_bit_vector<8>* generate_tree(const uint64_t amount) {
    auto tree = new buffered_packed_bit_vector<8>();

    for (uint64_t i = 0; i < amount; i++) {
        tree->push_back(i % 2);
    }
    return tree;
}

void do256(const __m256i* data, size_t amount) {
    cout << "data pointer: " << data << endl;
    for (size_t i = 0; i < amount; i++) {
        cout << static_cast<uint64_t>(_mm256_extract_epi64(data[i], 0)) << endl;
    }
}

int main() {
    /*vector<uint64_t> v;
    for (size_t i = 0; i < 64; i++) {
        v.push_back(i);
    }
    cout << "v pointer: " << &v[0] << endl;
    do256((__m256i*)&v[0], 4);
    //*/
    uint64_t size = 1000;
    auto tree = generate_tree(size);
    uint64_t sum = 0;
    for (uint64_t i = 320; i <= 323; i++) {
        auto val = tree->rank(i);
        sum += tree->at(i - 1);
        cout << "Rank " << i << " was " << val << "." << endl;
    }//*/
}
