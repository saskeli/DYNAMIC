#include <math.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include "dynamic/dynamic.hpp"

void help() {
    std::cout << "Tool for benchmarking rank operation speeds. If a n is\n"
                 "given, test procedures for bit vector of size n are\n"
                 "generated and output to std. If no size is given, test\n"
                 "procedure is read from std and restults output to std.\n\n";
    std::cout << "Usage: ./avx_comp <n>\n";
    std::cout << "   <n>   number of bits in the bitvector..\n";
    std::cout << "Example: benchmark avx.proc 10000000" << std::endl;
}

void write_procedure(uint64_t n) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::cout << n << std::endl;

    for (size_t i = 0; i < n; i++) {
        std::cout << (gen() % (i + 1)) << std::endl;
    }

    for (size_t i = 0; i < n; i++) {
        std::cout << (gen() % n) << std::endl;
    }
}

void run_test() {
    uint64_t n, v;
    std::cin >> n;

    dyn::b_suc_bv bv;
    for (size_t i = 0; i < n; i++) {
        std::cin >> v;
        bv.insert(v, i % 2);
    }

    std::vector<uint64_t> proc;

    for (size_t i = 0; i < n; i++) {
        std::cin >> v;
        proc.push_back(v);
    }

    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::microseconds;

    uint64_t checksum = 0;

    auto t1 = high_resolution_clock::now();
    for (size_t i = 0; i < n; i++) {
        checksum += bv.rank(proc[i]);
    }
    auto t2 = high_resolution_clock::now();
    std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / n
              << "\t" << checksum << std::endl;
}

int main(int argc, char const* argv[]) {
    if (argc == 2) {
        uint64_t n = atoi(argv[1]);
        write_procedure(n);
        return 0;
    } else if (argc == 1) {
        run_test();
        return 0;
    }
    help();
    return 0;
}
