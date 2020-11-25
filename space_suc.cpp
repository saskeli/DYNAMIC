#include <math.h>
#include <sys/resource.h>

#include <chrono>
#include <iomanip>
#include <random>
#include <vector>

#include "dynamic/dynamic.hpp"

template <class dyn_bv_t>
void benchmark_bv_ops(uint64_t size, uint64_t steps) {
    dyn_bv_t bv;

    struct rusage rusage;

    std::random_device rd;
    std::mt19937 gen(rd());

    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::microseconds;

    uint64_t ops = 100000;

    std::cout << "Size\t"
              << "Insert\t"
              << "Remove\t"
              << "bits size\t"
              << "resident set\t"
              << std::endl;

    for (uint64_t i = 0; i < size; i++) {
        bv.insert(gen() % (i + 1), gen() % 2);
    }

    for (size_t i = 0; i < steps; i++) {
        std::cout << size << "\t" << std::flush;

        auto t1 = high_resolution_clock::now();
        for (uint64_t j = 0; j < ops; j++) {
            bv.insert(gen() % (size + j + 1), gen() % 2);
        }
        auto t2 = high_resolution_clock::now();
        std::cout << ((double)duration_cast<microseconds>(t2 - t1).count() /
                      ops)
                  << "\t" << std::flush;

        t1 = high_resolution_clock::now();
        for (uint64_t j = 0; j < ops; j++) {
            bv.remove(gen() % bv.size());
        }
        t2 = high_resolution_clock::now();
        std::cout << ((double)duration_cast<microseconds>(t2 - t1).count() /
                      ops)
                  << "\t" << std::flush;

        std::cout << bv.bit_size() << "\t" << std::flush;

        if (!getrusage(RUSAGE_SELF, &rusage)) {
            std::cout << (size_t(rusage.ru_maxrss) * 1024 * 8) << "\n" << std::flush;
        } else {
            std::cout << "-1\n" << std::flush;
        }
    }
}

int main(int argc, char** argv) {
    uint64_t n = atoi(argv[1]);
    uint64_t s = atof(argv[2]);
    benchmark_bv_ops<dyn::b_suc_bv>(n, s);
}