#include <math.h>

#include <chrono>
#include <iomanip>
#include <random>
#include <vector>

#include "dynamic/dynamic.hpp"
#include "include/dynamic/internal/succinct_bitvector.hpp"

void help() {
    std::cout << "Benchmark some dynamic data structures of the library.\n"
              << "If type is present: Opreations as function of size "
                 "at steps points will be benchmarked.\n"
              << "If type is not present: Mixture speed transition "
                 "will be benchmarked\n\n";
    std::cout << "Usage: benchmark <-g|-s> <size> <steps>\n";
    std::cout << "   -s       benchmark succinct bitvector\n";
    std::cout << "   -b       benchmark buffered succinct bitvector\n";
    std::cout
        << "   -u       benchmark unbuffered buffered succinct bitvector\n";
    std::cout << "   <size>   number of bits in the bitvector\n";
    std::cout << "   <steps>  How many data points to generate in the "
                 "[0..size] range\n\n";
    std::cout << "Example: benchmark -s 1000000 100" << std::endl;

    exit(0);
}

void benchmark_bv_mix(uint64_t size, uint64_t steps) {
    dyn::b_suc_bv bbv;
    dyn::ub_suc_bv ubv;
    dyn::suc_bv sbv;

    std::random_device rd;
    std::mt19937 gen(rd());

    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::microseconds;

    uint64_t N = 100000;

    for (uint64_t i = 1; i <= size; i++) {
        uint64_t pos = gen() % i;
        uint64_t val = gen() % 2;
        bbv.insert(pos, val);
        sbv.insert(pos, val);
        ubv.insert(pos, val);
    }

    double step = 1.0 / (steps - 1);
    std::vector<uint64_t> ops;
    uint64_t checksum = 0;

    std::cout << "P\tcontrol\tbuffered\tunbuffered\tchecksum" << std::endl;

    for (uint64_t mul = 0; mul < steps; mul++) {
        double p = step * mul;
        std::cout << p << "\t";
        std::bernoulli_distribution bool_dist(p);
        ops.clear();
        checksum = 0;

        for (uint64_t opn = 0; opn < N; opn++) {
            ops.push_back(bool_dist(gen));
            ops.push_back(gen() % size);
        }

        auto t1 = high_resolution_clock::now();
        for (uint64_t opn = 0; opn < N; opn++) {
            if (ops[opn << 1]) {
                sbv.insert(ops[(opn << 1) + 1], 1);
            } else {
                checksum += sbv.rank(ops[(opn << 1) + 1]);
            }
        }
        auto t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / N
                  << "\t";

        checksum <<= 1;

        t1 = high_resolution_clock::now();
        for (uint64_t opn = 0; opn < N; opn++) {
            if (ops[opn << 1]) {
                bbv.insert(ops[(opn << 1) + 1], 1);
            } else {
                checksum -= bbv.rank(ops[(opn << 1) + 1]);
            }
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / N
                  << "\t";

        t1 = high_resolution_clock::now();
        for (uint64_t opn = 0; opn < N; opn++) {
            if (ops[opn << 1]) {
                ubv.insert(ops[(opn << 1) + 1], 1);
            } else {
                checksum -= ubv.rank(ops[(opn << 1) + 1]);
            }
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / N
                  << "\t";

        while (bbv.size() > size) {
            uint64_t pos = gen() % bbv.size();
            bbv.remove(pos);
            sbv.remove(pos);
            ubv.remove(pos);
        }

        std::cout << checksum << std::endl;
    }
}

template <class dyn_bv_t>
void benchmark_bv_ops(uint64_t size, uint64_t steps) {
    dyn_bv_t bv;

    std::random_device rd;
    std::mt19937 gen(rd());

    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::microseconds;

    double startexp = log2(double(10000.0));
    double delta = (log2(double(size)) - log2(10000.0)) / steps;
    uint64_t ops = 100000;
    std::cout << "startexp: " << startexp << ". delta: " << delta << std::endl;

    std::cout << "Size\tremove\tinsert\taccess\trank\tselect\tchecksum"
              << std::endl;

    for (uint64_t i = 0; i < 10000; i++) {
        bv.insert(gen() % (i + 1), gen() % 2);
    }

    for (uint64_t step = 1; step <= steps; step++) {
        uint64_t start = bv.size();
        uint64_t target = uint64_t(pow(2.0, startexp + delta * step));

        uint64_t checksum = 0;

        std::cout << target << "\t";

        auto t1 = high_resolution_clock::now();
        for (uint64_t i = start; i < target; i++) {
            bv.insert(gen() % (i + 1), gen() % 2);
        }
        auto t2 = high_resolution_clock::now();
        double insert_time =
            (double)duration_cast<microseconds>(t2 - t1).count() /
            (target - start);

        t1 = high_resolution_clock::now();
        for (uint64_t i = target; i > start; i--) {
            bv.remove(gen() % (i));
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() /
                         (target - start)
                  << "\t";

        t1 = high_resolution_clock::now();
        for (uint64_t i = start; i < target; i++) {
            bv.insert(gen() % (i + 1), gen() % 2);
        }
        t2 = high_resolution_clock::now();
        insert_time = (insert_time +
                       (double)duration_cast<microseconds>(t2 - t1).count() /
                           (target - start)) /
                      2;
        std::cout << insert_time << "\t";

        t1 = high_resolution_clock::now();
        for (uint64_t i = 0; i < ops; i++) {
            checksum += bv.at(gen() % target);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / ops
                  << "\t";

        t1 = high_resolution_clock::now();
        for (uint64_t i = 0; i < ops; i++) {
            checksum += bv.rank(gen() % target);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / ops
                  << "\t";

        uint64_t limit = bv.rank(target - 1);
        t1 = high_resolution_clock::now();
        for (uint64_t i = 0; i < ops; i++) {
            checksum += bv.select(gen() % limit);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / ops
                  << "\t";

        std::cout << checksum << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc == 3) {
        uint64_t n = atoi(argv[1]);
        uint64_t s = atof(argv[2]);
        std::cout << "Comparing operation speeds with different modification "
                     "probabilities"
                  << std::endl;
        benchmark_bv_mix(n, s);
    } else if (argc == 4) {
        uint64_t n = atoi(argv[2]);
        uint64_t s = atof(argv[3]);
        if (n <= 10000) {
            std::cout << "size should be significantly over 10000" << std::endl;
            return 1;
        }
        std::cout << "size = " << n << ". steps = " << s << std::endl;
        if (string(argv[1]).compare("-s") == 0) {
            std::cout << "Benchmarking succinct bitvector operations"
                      << std::endl;
            benchmark_bv_ops<dyn::suc_bv>(n, s);

        } else if (string(argv[1]).compare("-b") == 0) {
            std::cout << "Benchmarking buffered succinct bitvector operations"
                      << std::endl;
            benchmark_bv_ops<dyn::b_suc_bv>(n, s);
        } else if (string(argv[1]).compare("-u") == 0) {
            std::cout << "Benchmarking unbuffered buffered succinct bitvector "
                         "operations"
                      << std::endl;
            benchmark_bv_ops<dyn::ub_suc_bv>(n, s);
        } else {
            help();
        }
    } else {
        help();
    }
}