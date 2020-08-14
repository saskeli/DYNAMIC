from itertools import product
from os import listdir, remove

def generate(path, buffer, leaf, branch):
    header = """#include <math.h>
#include <sys/resource.h>

#include <chrono>
#include <iomanip>
#include <random>
#include <vector>

#include "../include/dynamic/internal/bufferedbv.hpp"
#include "../include/dynamic/internal/spsi.hpp"
#include "../include/dynamic/internal/succinct_bitvector.hpp"
"""
    body = r"""typedef dyn::succinct_bitvector<
    dyn::spsi<dyn::buffered_packed_bit_vector<BUF>, LEAF, BRANCH>>
    bv;

template <class dyn_bv_t>
void benchmark_bv_ops(uint64_t size, uint64_t steps) {
    dyn_bv_t bv;

    struct rusage rusage;

    std::random_device rd;
    std::mt19937 gen(rd());

    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::microseconds;

    double startexp = log2(double(100.0));
    double delta = (log2(double(size)) - log2(100.0)) / steps;
    uint64_t ops = 100000;

    std::cout << "Size\t"
              << "buffer size\t"
              << "leaf size\t"
              << "branching factor\t"
              << "bits size\t"
              << "resident set\t"
              << "remove\t"
              << "insert\t"
              << "access\t"
              << "rank\t"
              << "select\t"
              << "checksum" << std::endl;

    for (uint64_t i = 0; i < 100; i++) {
        bv.insert(gen() % (i + 1), gen() % 2);
    }

    for (uint64_t step = 1; step <= steps; step++) {
        uint64_t start = bv.size();
        uint64_t target = uint64_t(pow(2.0, startexp + delta * step));

        uint64_t checksum = 0;

        std::cout << target << "\t" << BUF << "\t" << LEAF << "\t" << BRANCH
                  << "\t";

        auto t1 = high_resolution_clock::now();
        for (uint64_t i = start; i < target; i++) {
            bv.insert(gen() % (i + 1), gen() % 2);
        }
        auto t2 = high_resolution_clock::now();
        double insert_time =
            (double)duration_cast<microseconds>(t2 - t1).count() /
            (target - start);

        std::cout << bv.size() << "\t";

        if (!getrusage(RUSAGE_SELF, &rusage)) {
            std::cout << (size_t(rusage.ru_maxrss) * 1024 * 8) << "\t";
        } else {
            std::cout << "-1\t";
        }

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
    uint64_t n = atoi(argv[1]);
    uint64_t s = atof(argv[2]);
    benchmark_bv_ops<bv>(n, s);
}"""

    with open(path, 'w') as out_file:
        out_file.write(header)
        out_file.write("\n#define BUF {}\n".format(buffer))
        out_file.write("#define LEAF {}\n".format(leaf))
        out_file.write("#define BRANCH {}\n".format(branch))
        out_file.write(body)

def main():
    buffers = [2*i for i in range(16)]
    leafs = [2**10 * i for i in range(16)]
    branches = [4 * (i + 1) for i in range(16)]

    for f in listdir("."):
        if f.endswith("cpp"):
            remove(f)
    
    with open("CMakeLists.txt", 'w') as out_file:
        for i, (buffer, leaf, branch) in enumerate(product(buffers, leafs, branches)):
            generate("t{}.cpp".format(i), buffer, leaf, branch)
            out_file.write("add_executable(\"t{}\" \"t{}.cpp\")\n".format(i, i))


if __name__ == "__main__":
    main()
