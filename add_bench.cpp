#include <math.h>

#include <chrono>
#include <iomanip>
#include <random>
#include <vector>
#include <stdio.h>

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
    std::vector<uint64_t> ops, loc, val;
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
            loc.push_back(gen() % size);
            val.push_back(gen() % 2);
        }

        auto t1 = high_resolution_clock::now();
        for (uint64_t opn = 0; opn < N; opn++) {
            if (ops[opn]) {
                sbv.insert(loc[opn], val[opn]);
            } else {
                checksum += sbv.rank(loc[opn]);
            }
        }
        auto t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / N
                  << "\t";

        checksum <<= 1;

        t1 = high_resolution_clock::now();
        for (uint64_t opn = 0; opn < N; opn++) {
            if (ops[opn]) {
                bbv.insert(loc[opn], val[opn]);
            } else {
                checksum -= bbv.rank(loc[opn]);
            }
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / N
                  << "\t";

        t1 = high_resolution_clock::now();
        for (uint64_t opn = 0; opn < N; opn++) {
            if (ops[opn]) {
                ubv.insert(loc[opn], val[opn]);
            } else {
                checksum -= ubv.rank(loc[opn]);
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
    std::mt19937 mt(rd());
    std::uniform_int_distribution<unsigned long long> gen(
        std::numeric_limits<std::uint64_t>::min(),
        std::numeric_limits<std::uint64_t>::max()
    );

    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::microseconds;

    uint64_t start_size = 1000000;

    double startexp = log2(double(start_size));
    double delta = (log2(double(size)) - log2(double(start_size))) / steps;
    uint64_t ops = 100000;
    std::cerr << "startexp: " << startexp << ". delta: " << delta << std::endl;
    std::vector<uint64_t> loc, val;

    std::cout << "Size\tremove\tinsert\taccess\trank\tselect\tsize(bits)\tchecksum"
              << std::endl;

    for (uint64_t i = 0; i < 900000; i++) {
        bv.insert(gen(mt) % (i + 1), gen(mt) % 2);
    }

    for (uint64_t step = 1; step <= steps; step++) {
        uint64_t start = bv.size();
        uint64_t target = uint64_t(pow(2.0, startexp + delta * step));

        uint64_t checksum = 0;

        std::cout << target << "\t";

        for (size_t i = start; i < target; i++) {
            bv.insert(gen(mt) % (i + 1), gen(mt) % 2);
        }

        loc.clear();
        for (uint64_t i = target; i > target - ops; i--) {
            loc.push_back(gen(mt) % i);
        }

        auto t1 = high_resolution_clock::now();
        for (size_t i = 0; i < ops; i++) {
            bv.remove(loc[i]);
        }
        auto t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / ops
                  << "\t";

        loc.clear();
        val.clear();
        for (uint64_t i = bv.size(); i < target; i++) {
            loc.push_back(gen(mt) % (i + 1));
            val.push_back(gen(mt) % 2);
        }

        t1 = high_resolution_clock::now();
        for (size_t i = 0; i < ops; i++) {
            bv.insert(loc[i], val[i]);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / ops
                  << "\t";

        loc.clear();
        for (size_t i = 0; i < ops; i++) {
            loc.push_back(gen(mt) % target);
        }

        t1 = high_resolution_clock::now();
        for (size_t i = 0; i < ops; i++) {
            checksum += bv.at(loc[i]);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / ops
                  << "\t";

        loc.clear();
        for (size_t i = 0; i < ops; i++) {
            loc.push_back(gen(mt) % target);
        }

        t1 = high_resolution_clock::now();
        for (size_t i = 0; i < ops; i++) {
            checksum += bv.rank(loc[i]);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / ops
                  << "\t";

        uint64_t limit = bv.rank(target - 1);
        loc.clear();
        for (size_t i = 0; i < ops; i++) {
            loc.push_back(gen(mt) % limit);
        }

        t1 = high_resolution_clock::now();
        for (size_t i = 0; i < ops; i++) {
            checksum += bv.select(loc[i]);
        }
        t2 = high_resolution_clock::now();

        std::cout << (double)duration_cast<microseconds>(t2 - t1).count() / ops
                  << "\t";

        std::cout << bv.bit_size() << "\t";

        std::cout << checksum << std::endl;
    }
}

int main(int argc, char** argv) {
    uint64_t n;
    uint64_t s;
    if (argc == 3) {
        std::sscanf(argv[1], "%lu", &n);
        std::sscanf(argv[2], "%lu", &s);
        std::cerr << "Comparing operation speeds with different modification "
                     "probabilities for " << n << " element vector in "
                  << s << " steps " << std::endl;
        benchmark_bv_mix(n, s);
    } else if (argc == 4) {
        std::sscanf(argv[2], "%lu", &n);
        std::sscanf(argv[3], "%lu", &s);
        if (n < 1000000) {
            std::cerr << "size should be greater than or equal to 1000000"
                      << std::endl;
            return 1;
        }
        if (string(argv[1]).compare("-s") == 0) {
            std::cerr << "Benchmarking succinct bitvector operations up to "
                      << n << " elements in " << s << " steps" << std::endl;
            benchmark_bv_ops<dyn::suc_bv>(n, s);

        } else if (string(argv[1]).compare("-b") == 0) {
            std::cerr
                << "Benchmarking buffered succinct bitvector operations up to "
                << n << " elements in " << s << " steps" << std::endl;
            benchmark_bv_ops<dyn::b_suc_bv>(n, s);
        } else if (string(argv[1]).compare("-u") == 0) {
            std::cerr << "Benchmarking unbuffered buffered succinct bitvector "
                         "operations up to "
                      << n << " elements in " << s << " steps" << std::endl;
            benchmark_bv_ops<dyn::ub_suc_bv>(n, s);
        } else {
            help();
        }
    } else {
        help();
    }
}