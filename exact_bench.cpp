#include <math.h>

#include <chrono>
#include <iomanip>
#include <random>
#include <vector>
#include <stdio.h>

#include "dynamic/dynamic.hpp"
#include "include/dynamic/internal/succinct_bitvector.hpp"

void help() {
    std::cout << "Compare exact execution times of different operations.\n\n";
    std::cout << "Usage: benchmark <size> <n_ops>\n";
    std::cout << "   <size>   number of bits in the initial bitvectors\n";
    std::cout << "   <n_ops>  number of random operations to run\n\n";
    std::cout << "Example: benchmark 1000000 1000000" << std::endl;

    exit(0);
}

template<class BV>
uint64_t execute_op(uint8_t op, uint64_t loc, uint64_t val, BV ds) {
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::nanoseconds;
    uint64_t ret = 0;
    double t;
    if (op == 0) {
        auto t1 = high_resolution_clock::now();
        ds.insert(loc, val);
        auto t2 = high_resolution_clock::now();
        t = (double)duration_cast<nanoseconds>(t2 - t1).count();
    } else if (op == 1) {
        auto t1 = high_resolution_clock::now();
        ret = ds.at(loc);
        auto t2 = high_resolution_clock::now();
        t = (double)duration_cast<nanoseconds>(t2 - t1).count();
    } else if (op == 2) {
        auto t1 = high_resolution_clock::now();
        ds.remove(loc);
        auto t2 = high_resolution_clock::now();
        t = (double)duration_cast<nanoseconds>(t2 - t1).count();
    } else if (op == 3) {
        auto t1 = high_resolution_clock::now();
        ret = ds.rank(loc);
        auto t2 = high_resolution_clock::now();
        t = (double)duration_cast<nanoseconds>(t2 - t1).count();
    } else {
        auto t1 = high_resolution_clock::now();
        ret = ds.select(loc);
        auto t2 = high_resolution_clock::now();
        t = (double)duration_cast<nanoseconds>(t2 - t1).count();
    }
    std::cout << std::setw(8) << (t / 1000) << "\t";
    return ret;
}

void benchmark_bv_ops(uint64_t size, uint64_t n_ops) {
    dyn::suc_bv sbv;
    dyn::b_suc_bv b_sbv;
    dyn::ub_suc_bv ub_sbv;

    std::random_device rd;
    std::mt19937 gen(rd());

    for (uint64_t i = 0; i < size; i++) {
        uint64_t loc = gen() % (i + 1);
        uint64_t val = gen() % 2;
        sbv.insert(loc, val);
        b_sbv.insert(loc, val);
        ub_sbv.insert(loc, val);
    }

    std::cout << "Size\top\tloc\tDYNAMIC\tbuffered\tunbuffered" << std::endl;
    uint64_t ds_size = size;
    for (size_t i = 0; i < n_ops; i++) {
        std::cout << ds_size << "\t";
        uint8_t op = gen() % 5;
        uint64_t val = gen() % 2;
        uint64_t loc;
        if (op == 0) {
            loc = gen() % (ds_size + 1);
            std::cout << "insert\t";
        } else if (op == 4) {
            uint64_t n_elem = ub_sbv.rank(ub_sbv.size());
            loc = gen() % n_elem;
            std::cout << "select\t";
        } else {
            loc = gen() % ds_size;
            if (op == 1) std::cout << "access\t";
            else if (op == 2) std::cout << "remove\t";
            else std::cout << "rank\t";
        }
        std::cout << loc << "\t";

        uint64_t ret_a = execute_op<dyn::suc_bv>(op, loc, val, sbv);
        uint64_t ret_b = execute_op<dyn::b_suc_bv>(op, loc, val, b_sbv);
        uint64_t ret_c = execute_op<dyn::ub_suc_bv>(op, loc, val, ub_sbv);

        std::cout << std::endl;
        if (ret_a != ret_b || ret_b != ret_c) {
            std::cerr << "Checksum error... terminating" << std::endl;
            exit(1);
        }
        if (op == 0) ds_size++;
        else if (op == 2) ds_size--;
    }
}

int main(int argc, char** argv) {
    uint64_t n;
    uint64_t n_ops;
    if (argc == 3) {
        std::sscanf(argv[1], "%lu", &n);
        std::sscanf(argv[2], "%lu", &n_ops);
        if (n < 1000000) {
            std::cout << "size should be greater than or equal to 1000000"
                      << std::endl;
            return 1;
        }
        benchmark_bv_ops(n, n_ops);
    } else {
        help();
    }
}