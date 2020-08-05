// Copyright (c) 2017, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 * benchmark.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: nico
 */

#include <chrono>

#include "dynamic/algorithms/rle_lz77_v1.hpp"
#include "dynamic/algorithms/rle_lz77_v2.hpp"
#include "dynamic/dynamic.hpp"
#include "dynamic/internal/alphabet_encoder.hpp"
#include "dynamic/internal/packed_vector.hpp"
#include "dynamic/internal/spsi.hpp"
#include "dynamic/internal/spsi_check.hpp"
#include "dynamic/internal/wt_string.hpp"

using namespace std;
using namespace dyn;

void help() {
    cout << "Benchmark some dynamic data structures of the library." << endl
         << endl;
    cout << "Usage: benchmark <-g|-s> <size> <P>" << endl;
    cout << "   -g       benchmark gap bitvector" << endl;
    cout << "   -s       benchmark succinct bitvector" << endl;
    cout << "   -b       benchmark buffered succinct bitvector" << endl;
    cout << "   <size>   number of bits in the bitvector" << endl;
    cout << "   <P>      probability of a bit set in [0,1]" << endl << endl;
    cout << "Example: benchmark -g 1000000 0.01" << endl;

    exit(0);
}

template <class dyn_bv_t>
void benchmark_bv(uint64_t size, double p = 0.5) {
    dyn_bv_t bv;

    srand(time(NULL));

    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;

    cout << "insert ... " << flush;
    auto t1 = high_resolution_clock::now();
    for (uint64_t i = 0; i < size; ++i) {
        ulint c = double(rand()) / RAND_MAX < p ? 1 : 0;
        bv.insert(rand() % (bv.size() + 1), c);
    }
    auto t2 = high_resolution_clock::now();
    uint64_t sec_insert =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    cout << "done." << endl;

    auto max_size = bv.bit_size();

    cout << "access ... " << flush;
    t1 = high_resolution_clock::now();
    for (uint64_t i = 0; i < size; ++i) {
        bv.at(rand() % bv.size());
    }
    t2 = high_resolution_clock::now();
    uint64_t sec_access =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    cout << "done.\n";

    cout << "rank 0 ... " << flush;
    t1 = high_resolution_clock::now();
    for (uint64_t i = 0; i < size; ++i) {
        bv.rank(rand() % (bv.size() + 1), 0);
    }
    t2 = high_resolution_clock::now();
    uint64_t sec_rank0 =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    cout << "done.\n";

    cout << "rank 1 ... " << flush;
    t1 = high_resolution_clock::now();
    for (uint64_t i = 0; i < size; ++i) {
        bv.rank(rand() % (bv.size() + 1), 1);
    }
    t2 = high_resolution_clock::now();
    uint64_t sec_rank1 =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    cout << "done." << endl;

    uint64_t nr_0 = bv.rank(bv.size(), 0);
    uint64_t nr_1 = bv.rank(bv.size(), 1);

    cout << "select 0 ... " << flush;
    t1 = high_resolution_clock::now();
    for (uint64_t i = 0; i < size; ++i) {
        bv.select(rand() % nr_0, 0);
    }
    t2 = high_resolution_clock::now();
    uint64_t sec_sel0 =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    cout << "done.\n";

    cout << "select 1 ... " << flush;
    t1 = high_resolution_clock::now();
    for (uint64_t i = 0; i < size; ++i) {
        bv.select(rand() % nr_1, 1);
    }
    t2 = high_resolution_clock::now();
    uint64_t sec_sel1 =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    cout << "done.\n";

    cout << "remove ... " << flush;
    t1 = high_resolution_clock::now();
    for (uint64_t i = 0; i < size; ++i) {
        bv.remove(rand() % bv.size());
    }
    t2 = high_resolution_clock::now();
    uint64_t sec_rem =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    cout << "done." << endl;

    cout << (double)sec_insert / size << " microseconds/insert" << endl;
    cout << (double)sec_access / size << " microseconds/access" << endl;
    cout << (double)sec_rank0 / size << " microseconds/rank0" << endl;
    cout << (double)sec_rank1 / size << " microseconds/rank1" << endl;
    cout << (double)sec_sel0 / size << " microseconds/select0" << endl;
    cout << (double)sec_sel1 / size << " microseconds/select1" << endl;

    cout << (double)sec_rem / size << " microseconds/remove" << endl;

    cout << "Max bit size of the structure (allocated memory, bits): "
         << max_size << endl;
    cout << "Final bit size of the structure (allocated memory, bits): "
         << bv.bit_size() << endl;
}

int main(int argc, char** argv) {
    if (argc != 4) help();

    ulint n = atoi(argv[2]);
    double P = atof(argv[3]);

    cout << "size = " << n << ". P = " << P << endl;

    if (string(argv[1]).compare("-g") == 0) {
        cout << "Benchmarking gap bitvector" << endl;
        benchmark_bv<gap_bv>(n, P);

    } else if (string(argv[1]).compare("-s") == 0) {
        cout << "Benchmarking succinct bitvector" << endl;
        benchmark_bv<suc_bv>(n, P);

    } else if (string(argv[1]).compare("-b") == 0) {
        cout << "Benchmarking buffered succinct bitvector" << endl;
        benchmark_bv<b_suc_bv>(n, P);
    } else
        help();
}
