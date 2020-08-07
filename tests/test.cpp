#include "../include/dynamic/dynamic.hpp"
#include "../include/dynamic/internal/bufferedbv.hpp"
#include "gtest.h"
#include "helpers.hpp"
#include "../include/dynamic/internal/spsi.hpp"
#include "../include/dynamic/internal/succinct_bitvector.hpp"

using namespace dyn;

typedef buffered_packed_bit_vector<8> pv;
typedef buffered_packed_bit_vector<0> pv0;
typedef succinct_bitvector<spsi<buffered_packed_bit_vector<0>,8192,16>> b_suc_bv0;

TEST(PV, push_back) { pv_pushback_test<pv>(); }

TEST(PV, insert) { pv_insert_test<pv>(); }

TEST(PV, remove) { pv_remove_test<pv>(); }

TEST(PV, Insertion10) { insert_test<pv>(10); }

TEST(PV, Insertion100) { insert_test<pv>(100); }

TEST(PV, Insertion1000) { insert_test<pv>(1000); }

TEST(PV, Insertion10000) { insert_test<pv>(10000); }

TEST(PV, Mixture10) { mixture_test<pv>(10); }

TEST(PV, Mixture100) { mixture_test<pv>(100); }

TEST(PV, Mixture1000) { mixture_test<pv>(1000); }

TEST(PV, Mixture10000) { mixture_test<pv>(10000); }

TEST(PV, Rank10) { rank_test<pv>(10); }

TEST(PV, Rank100) { rank_test<pv>(100); }

TEST(PV, Rank1000) { rank_test<pv>(1000); }

TEST(PV, Rank10000) { rank_test<pv>(10000); }

TEST(PV, Remove10) { remove_test<pv>(10); }

TEST(PV, Remove100) { remove_test<pv>(100); }

TEST(PV, Remove1000) { remove_test<pv>(1000); }

TEST(PV, Remove10000) { remove_test<pv>(10000); }

TEST(PV, Update10) { update_test<pv>(10); }

TEST(PV, Update100) { update_test<pv>(100); }

TEST(PV, Update1000) { update_test<pv>(1000); }

TEST(PV, Update10000) { update_test<pv>(10000); }

TEST(PV, Select10) { select_test<pv>(10); }

TEST(PV, Select100) { select_test<pv>(100); }

TEST(PV, Select1000) { select_test<pv>(1000); }

TEST(PV, Select10000) { select_test<pv>(10000); }

TEST(BBV8, Random1) {
    std::vector<uint32_t> ops{3, 1, 0, 0, 1, 1, 2, 0, 0, 1,
                              0, 2, 0, 1, 3, 0, 0, 0, 1};
    run_test<b_suc_bv>(ops);
}

TEST(BBV8, Random2) {
    std::vector<uint32_t> ops{1, 3, 1, 1, 1, 1, 0, 2, 0, 1, 3,
                              0, 1, 0, 3, 0, 0, 0, 1, 5, 0};
    run_test<b_suc_bv>(ops);
}

TEST(BBV8, Random3) {
    std::vector<uint32_t> ops{47, 3, 1,     5, 15391, 4, 19, 3, 0, 5, 10556, 4,
                              47, 5, 27092, 5, 24392, 4, 3,  0, 3, 1};
    run_test<b_suc_bv>(ops);
}

TEST(BBV8, Random4) {
    std::vector<uint32_t> ops{98, 5,  21266, 1, 64, 2, 9, 1,  3, 1, 5, 26631,
                              0,  87, 1,     2, 94, 0, 1, 63, 3, 1, 5, 4707};
    run_test<b_suc_bv>(ops);
}

TEST(BBV8, Random5) {
    std::vector<uint32_t> ops{34, 1,  5, 5, 54003, 1, 5, 3, 1,     3, 1,
                              4,  19, 3, 0, 0,     5, 1, 5, 17609, 4, 8};
    run_test<b_suc_bv>(ops);
}

TEST(BBV8, Random6) {
    std::vector<uint32_t> ops{5, 2, 0, 1, 4, 0, 1, 2, 0, 1,     1, 3,
                              1, 1, 2, 4, 1, 0, 2, 0, 5, 53251, 4, 5};
    run_test<b_suc_bv>(ops);
}

TEST(BBV8, Insertion10) { insert_test<b_suc_bv>(10); }

TEST(BBV8, Insertion100) { insert_test<b_suc_bv>(100); }

TEST(BBV8, Insertion1000) { insert_test<b_suc_bv>(1000); }

TEST(BBV8, Insertion10000) { insert_test<b_suc_bv>(10000); }

TEST(BBV8, Insertion100000) { insert_test<b_suc_bv>(100000); }

TEST(BBV8, Insertion1000000) { insert_test<b_suc_bv>(1000000); }

TEST(BBV8, Mixture10) { mixture_test<b_suc_bv>(10); }

TEST(BBV8, Mixture100) { mixture_test<b_suc_bv>(100); }

TEST(BBV8, Mixture1000) { mixture_test<b_suc_bv>(1000); }

TEST(BBV8, Mixture10000) { mixture_test<b_suc_bv>(10000); }

TEST(BBV8, Rank10) { rank_test<b_suc_bv>(10); }

TEST(BBV8, Rank100) { rank_test<b_suc_bv>(100); }

TEST(BBV8, Rank1000) { rank_test<b_suc_bv>(1000); }

TEST(BBV8, Rank10000) { rank_test<b_suc_bv>(10000); }

TEST(BBV8, Rank100000) { rank_test<b_suc_bv>(100000); }

TEST(BBV8, Rank1000000) { rank_test<b_suc_bv>(1000000); }

TEST(BBV8, Remove10) { remove_test<b_suc_bv>(10); }

TEST(BBV8, Remove100) { remove_test<b_suc_bv>(100); }

TEST(BBV8, Remove1000) { remove_test<b_suc_bv>(1000); }

TEST(BBV8, Remove10000) { remove_test<b_suc_bv>(10000); }

TEST(BBV8, Remove100000) { remove_test<b_suc_bv>(100000); }

TEST(BBV8, Remove1000000) { remove_test<b_suc_bv>(1000000); }

TEST(BBV8, Update10) { update_test<b_suc_bv>(10); }

TEST(BBV8, Update100) { update_test<b_suc_bv>(100); }

TEST(BBV8, Update1000) { update_test<b_suc_bv>(1000); }

TEST(BBV8, Update10000) { update_test<b_suc_bv>(10000); }

TEST(BBV8, Update100000) { update_test<b_suc_bv>(100000); }

TEST(BBV8, Update1000000) { update_test<b_suc_bv>(1000000); }

TEST(BBV8, Select10) { select_test<b_suc_bv>(10); }

TEST(BBV8, Select100) { select_test<b_suc_bv>(100); }

TEST(BBV8, Select1000) { select_test<b_suc_bv>(1000); }

TEST(BBV8, Select10000) { select_test<b_suc_bv>(10000); }

TEST(BBV8, Select100000) { select_test<b_suc_bv>(100000); }

TEST(BBV8, Select1000000) { select_test<b_suc_bv>(1000000); }

TEST(PV0, push_back) { pv_pushback_test<pv0>(); }

TEST(PV0, insert) { pv_insert_test<pv0>(); }

TEST(PV0, remove) { pv_remove_test<pv0>(); }

TEST(PV0, Insertion10) { insert_test<pv0>(10); }

TEST(PV0, Insertion100) { insert_test<pv0>(100); }

TEST(PV0, Insertion1000) { insert_test<pv0>(1000); }

TEST(PV0, Insertion10000) { insert_test<pv0>(10000); }

TEST(PV0, Mixture10) { mixture_test<pv0>(10); }

TEST(PV0, Mixture100) { mixture_test<pv0>(100); }

TEST(PV0, Mixture1000) { mixture_test<pv0>(1000); }

TEST(PV0, Mixture10000) { mixture_test<pv0>(10000); }

TEST(PV0, Rank10) { rank_test<pv0>(10); }

TEST(PV0, Rank100) { rank_test<pv0>(100); }

TEST(PV0, Rank1000) { rank_test<pv0>(1000); }

TEST(PV0, Rank10000) { rank_test<pv0>(10000); }

TEST(PV0, Remove10) { remove_test<pv0>(10); }

TEST(PV0, Remove100) { remove_test<pv0>(100); }

TEST(PV0, Remove1000) { remove_test<pv0>(1000); }

TEST(PV0, Remove10000) { remove_test<pv0>(10000); }

TEST(PV0, Update10) { update_test<pv0>(10); }

TEST(PV0, Update100) { update_test<pv0>(100); }

TEST(PV0, Update1000) { update_test<pv0>(1000); }

TEST(PV0, Update10000) { update_test<pv0>(10000); }

TEST(PV0, Select10) { select_test<pv0>(10); }

TEST(PV0, Select100) { select_test<pv0>(100); }

TEST(PV0, Select1000) { select_test<pv0>(1000); }

TEST(PV0, Select10000) { select_test<pv0>(10000); }

TEST(BBV0, Random1) {
    std::vector<uint32_t> ops{3, 1, 0, 0, 1, 1, 2, 0, 0, 1,
                              0, 2, 0, 1, 3, 0, 0, 0, 1};
    run_test<b_suc_bv0>(ops);
}

TEST(BBV0, Random2) {
    std::vector<uint32_t> ops{1, 3, 1, 1, 1, 1, 0, 2, 0, 1, 3,
                              0, 1, 0, 3, 0, 0, 0, 1, 5, 0};
    run_test<b_suc_bv0>(ops);
}

TEST(BBV0, Random3) {
    std::vector<uint32_t> ops{47, 3, 1,     5, 15391, 4, 19, 3, 0, 5, 10556, 4,
                              47, 5, 27092, 5, 24392, 4, 3,  0, 3, 1};
    run_test<b_suc_bv0>(ops);
}

TEST(BBV0, Random4) {
    std::vector<uint32_t> ops{98, 5,  21266, 1, 64, 2, 9, 1,  3, 1, 5, 26631,
                              0,  87, 1,     2, 94, 0, 1, 63, 3, 1, 5, 4707};
    run_test<b_suc_bv0>(ops);
}

TEST(BBV0, Random5) {
    std::vector<uint32_t> ops{34, 1,  5, 5, 54003, 1, 5, 3, 1,     3, 1,
                              4,  19, 3, 0, 0,     5, 1, 5, 17609, 4, 8};
    run_test<b_suc_bv0>(ops);
}

TEST(BBV0, Random6) {
    std::vector<uint32_t> ops{5, 2, 0, 1, 4, 0, 1, 2, 0, 1,     1, 3,
                              1, 1, 2, 4, 1, 0, 2, 0, 5, 53251, 4, 5};
    run_test<b_suc_bv0>(ops);
}

TEST(BBV0, Insertion10) { insert_test<b_suc_bv0>(10); }

TEST(BBV0, Insertion100) { insert_test<b_suc_bv0>(100); }

TEST(BBV0, Insertion1000) { insert_test<b_suc_bv0>(1000); }

TEST(BBV0, Insertion10000) { insert_test<b_suc_bv0>(10000); }

TEST(BBV0, Insertion100000) { insert_test<b_suc_bv0>(100000); }

TEST(BBV0, Insertion1000000) { insert_test<b_suc_bv0>(1000000); }

TEST(BBV0, Mixture10) { mixture_test<b_suc_bv0>(10); }

TEST(BBV0, Mixture100) { mixture_test<b_suc_bv0>(100); }

TEST(BBV0, Mixture1000) { mixture_test<b_suc_bv0>(1000); }

TEST(BBV0, Mixture10000) { mixture_test<b_suc_bv0>(10000); }

TEST(BBV0, Rank10) { rank_test<b_suc_bv0>(10); }

TEST(BBV0, Rank100) { rank_test<b_suc_bv0>(100); }

TEST(BBV0, Rank1000) { rank_test<b_suc_bv0>(1000); }

TEST(BBV0, Rank10000) { rank_test<b_suc_bv0>(10000); }

TEST(BBV0, Rank100000) { rank_test<b_suc_bv0>(100000); }

TEST(BBV0, Rank1000000) { rank_test<b_suc_bv0>(1000000); }

TEST(BBV0, Remove10) { remove_test<b_suc_bv0>(10); }

TEST(BBV0, Remove100) { remove_test<b_suc_bv0>(100); }

TEST(BBV0, Remove1000) { remove_test<b_suc_bv0>(1000); }

TEST(BBV0, Remove10000) { remove_test<b_suc_bv0>(10000); }

TEST(BBV0, Remove100000) { remove_test<b_suc_bv0>(100000); }

TEST(BBV0, Remove1000000) { remove_test<b_suc_bv0>(1000000); }

TEST(BBV0, Update10) { update_test<b_suc_bv0>(10); }

TEST(BBV0, Update100) { update_test<b_suc_bv0>(100); }

TEST(BBV0, Update1000) { update_test<b_suc_bv0>(1000); }

TEST(BBV0, Update10000) { update_test<b_suc_bv0>(10000); }

TEST(BBV0, Update100000) { update_test<b_suc_bv0>(100000); }

TEST(BBV0, Update1000000) { update_test<b_suc_bv0>(1000000); }

TEST(BBV0, Select10) { select_test<b_suc_bv0>(10); }

TEST(BBV0, Select100) { select_test<b_suc_bv0>(100); }

TEST(BBV0, Select1000) { select_test<b_suc_bv0>(1000); }

TEST(BBV0, Select10000) { select_test<b_suc_bv0>(10000); }

TEST(BBV0, Select100000) { select_test<b_suc_bv0>(100000); }

TEST(BBV0, Select1000000) { select_test<b_suc_bv0>(1000000); }