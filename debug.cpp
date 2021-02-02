#include <immintrin.h>

#include <chrono>
#include <cstdlib>
#include <iostream>

#include "dynamic/dynamic.hpp"
#include "dynamic/internal/bufferedbv.hpp"

using namespace std;
using namespace dyn;

typedef dyn::buffered_packed_bit_vector<8> pv;

b_suc_bv* generate_tree(const uint64_t amount) {
    auto tree = new b_suc_bv();

   dyn::succinct_spsi spsi;
   dyn::spsi_check<> check;

   int n=100000;
   int sigma=120;

   srand(time(NULL));

   for(int i=0;i<n;++i){

	   cout << "insert number " << i << endl;

	   int j = rand()%(i+1);
	   int x = rand()%sigma;

	   spsi.insert(j,x);
	   check.insert(j,x);

   }

    for (uint64_t i = 0; i < amount; i++) {
        tree->push_back(i % 2);
    }
    return tree;
}

suc_bv* generate_c_tree(const uint64_t amount) {
    auto tree = new suc_bv();

    for (uint64_t i = 0; i < amount; i++) {
        tree->push_back(i % 2);
    }
    return tree;
}

pv* generate_pv(const uint64_t amount) {
    auto tree = new pv();
    for (uint64_t i = 0; i < amount; i++) {
        tree->push_back(i % 2);
    }
    return tree;
}

int main() {
    std::vector<uint32_t> ops{98, 5,  21266, 1, 64, 2, 9, 1,  3, 1, 5, 26631,
                              0,  87, 1,     2, 94, 0, 1, 63, 3, 1, 5, 4707};
    auto buffered_tree = generate_tree(ops[0]);
    auto control_tree = generate_c_tree(ops[0]);

    size_t i = 1;
    while (i < ops.size()) {
        uint8_t step = 0;
        uint32_t selection = ops[i];
        uint64_t s = 0;
        switch (selection) {
            case 0:
                s = buffered_tree->size();
                buffered_tree->insert(s ? ops[i + 1] % s : 0, ops[i + 2]);
                control_tree->insert(s ? ops[i + 1] % s : 0, ops[i + 2]);
                step = 3;
                break;
            case 1:
                s = buffered_tree->size();
                step = 2;
                if (s == 0) break;
                buffered_tree->remove(ops[i + 1] % buffered_tree->size());
                control_tree->remove(ops[i + 1] % control_tree->size());
                break;
            case 2:
                s = buffered_tree->size();
                step = 3;
                if (s == 0) break;
                buffered_tree->set(ops[i + 1] % buffered_tree->size(),
                                   ops[i + 2]);
                control_tree->set(ops[i + 1] % control_tree->size(),
                                  ops[i + 2]);
                break;
            case 3:
                buffered_tree->push_back(ops[i + 1]);
                control_tree->push_back(ops[i + 1]);
                step = 2;
                break;
            case 4: {
                s = buffered_tree->size();
                step = 2;
                if (s == 0) break;
                uint64_t ic_r = ops[i + 1] % control_tree->size();
                uint64_t ib_r = ops[i + 1] % buffered_tree->size();
                if (ic_r != ib_r) {
                    std::cout << "Unexpected tree size. " << ops[i + 1] << " % tree_size should be " << ic_r << ". was " << ib_r << std::endl;
                    exit(1);
                }
                uint64_t b_r = buffered_tree->rank(ic_r);
                uint64_t c_r = control_tree->rank(ic_r);
                if (b_r != c_r) {
                    std::cout << "Unexpected result of rank(" << ic_r << ") query at "
                    << i << ". Expected " << c_r << ", got " << b_r << std::endl;
                    exit(1);
                }
            } break;
            default:
                s = buffered_tree->size();
                step = 2;
                if (s == 0) break;
                uint64_t ba_r = buffered_tree->rank(buffered_tree->size());
                uint64_t ca_r = control_tree->rank(control_tree->size());
                if (ba_r != ca_r) {
                    std::cout << "Unexpected one count for rank query. Expected " << ca_r
                    << ", got " << ba_r << std::endl;
                    exit(1);
                }
                if (ba_r == 0) break;
                auto loc = ops[i + 1] % ba_r;
                uint64_t b_s = buffered_tree->select(loc);
                uint64_t c_s = control_tree->select(loc);
                if (b_s != c_s) {
                    std::cout << i << ": Unexpected result of select(" << loc
                    << ") query. Expected " << c_s << ", got " << b_s << std::endl;
                    exit(1);
                }
                break;
        }
        uint64_t size = control_tree->size();
        if (size != buffered_tree->size()) {
            std::cout << "Expected vector size of " << size << " but found "
            << buffered_tree->size() << std::endl;
            exit(1);
        }
        for (size_t idx = 0; idx < size; idx++) {
            if (control_tree->at(idx) != buffered_tree->at(idx)) {
                std::cout << "Expected " << control_tree->at(idx) << " at position "
                << idx << " after op " << i << std::endl;
                exit(1);
            }
        }
        i += step;
    }
}
