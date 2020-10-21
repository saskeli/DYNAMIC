#include "../include/dynamic/dynamic.hpp"
#include "../include/dynamic/internal/bufferedbv.hpp"
#include "gtest.h"
#include "helpers.hpp"
#include "../include/dynamic/internal/spsi.hpp"
#include "../include/dynamic/internal/succinct_bitvector.hpp"

using namespace dyn;

typedef buffered_packed_bit_vector<8> pv;
typedef buffered_packed_bit_vector<0> pv0;

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

TEST(BBV8, Random7) {
    std::vector<uint32_t> ops{233, 3, 0, 0, 140, 1, 0, 129, 1, 1, 16, 2, 61, 
        0, 3, 1, 2, 50, 1, 2, 156, 1, 0, 199, 1, 4, 68, 3, 1, 3, 1, 4, 183, 
        3, 0, 1, 54, 1, 173, 0, 221, 1, 3, 0, 6, 91, 2, 47, 0, 2, 85, 0, 0, 
        66, 0, 6, 65, 4, 161, 2, 228, 0, 6, 54, 2, 113, 0, 6, 82, 1, 147, 4, 
        172, 5, 4259669018, 2, 85, 0, 0, 131, 1, 2, 118, 0, 6, 96, 3, 0, 0, 
        33, 1, 2, 16, 0, 0, 239, 0, 1, 44, 2, 198, 0, 0, 58, 1, 5, 63944229, 
        6, 40, 3, 1, 1, 129, 2, 129, 1, 4, 4, 6, 0, 5, 1709272325, 0, 180, 0, 
        4, 241, 2, 114, 0, 2, 158, 1, 0, 236, 1, 1, 180, 0, 196, 0, 5, 
        2064179465, 2, 245, 0, 5, 3519718982, 5, 2615211358, 1, 68, 0, 203, 
        1, 5, 3247946259, 3, 0, 1, 57, 3, 1, 1, 198, 1, 107, 4, 157, 2, 29, 
        1, 2, 83, 0, 6, 241, 3, 0, 4, 3, 3, 1, 2, 90, 1, 4, 121, 4, 8, 6, 70, 
        1, 155, 4, 122, 2, 9, 0, 0, 126, 1, 4, 60, 6, 210, 2, 149, 0, 1, 118, 
        1, 91, 4, 14, 6, 97, 6, 82, 6, 63, 6, 46, 5, 736584181, 6, 228, 5, 
        3449578994, 2, 169, 0, 1, 147, 0, 12, 1};
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

TEST(BBV8, Rank0_10) { rank0_test<b_suc_bv>(10); }

TEST(BBV8, Rank0_100) { rank0_test<b_suc_bv>(100); }

TEST(BBV8, Rank0_1000) { rank0_test<b_suc_bv>(1000); }

TEST(BBV8, Rank0_10000) { rank0_test<b_suc_bv>(10000); }

TEST(BBV8, Rank0_100000) { rank0_test<b_suc_bv>(100000); }

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

TEST(BBV8, Select0_10) { select0_test<b_suc_bv>(10); }

TEST(BBV8, Select0_100) { select0_test<b_suc_bv>(100); }

TEST(BBV8, Select0_1000) { select0_test<b_suc_bv>(1000); }

TEST(BBV8, Select0_10000) { select0_test<b_suc_bv>(10000); }

TEST(BBV8, Select0_100000) { select0_test<b_suc_bv>(100000); }

TEST(BBV8, Select0_1000000) { select0_test<b_suc_bv>(1000000); }

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
    run_test<ub_suc_bv>(ops);
}

TEST(BBV0, Random2) {
    std::vector<uint32_t> ops{1, 3, 1, 1, 1, 1, 0, 2, 0, 1, 3,
                              0, 1, 0, 3, 0, 0, 0, 1, 5, 0};
    run_test<ub_suc_bv>(ops);
}

TEST(BBV0, Random3) {
    std::vector<uint32_t> ops{47, 3, 1,     5, 15391, 4, 19, 3, 0, 5, 10556, 4,
                              47, 5, 27092, 5, 24392, 4, 3,  0, 3, 1};
    run_test<ub_suc_bv>(ops);
}

TEST(BBV0, Random4) {
    std::vector<uint32_t> ops{98, 5,  21266, 1, 64, 2, 9, 1,  3, 1, 5, 26631,
                              0,  87, 1,     2, 94, 0, 1, 63, 3, 1, 5, 4707};
    run_test<ub_suc_bv>(ops);
}

TEST(BBV0, Random5) {
    std::vector<uint32_t> ops{34, 1,  5, 5, 54003, 1, 5, 3, 1,     3, 1,
                              4,  19, 3, 0, 0,     5, 1, 5, 17609, 4, 8};
    run_test<ub_suc_bv>(ops);
}

TEST(BBV0, Random6) {
    std::vector<uint32_t> ops{5, 2, 0, 1, 4, 0, 1, 2, 0, 1,     1, 3,
                              1, 1, 2, 4, 1, 0, 2, 0, 5, 53251, 4, 5};
    run_test<ub_suc_bv>(ops);
}

TEST(BBV0, Random7) {
    std::vector<uint32_t> ops{233, 3, 0, 0, 140, 1, 0, 129, 1, 1, 16, 2, 61, 
        0, 3, 1, 2, 50, 1, 2, 156, 1, 0, 199, 1, 4, 68, 3, 1, 3, 1, 4, 183, 
        3, 0, 1, 54, 1, 173, 0, 221, 1, 3, 0, 6, 91, 2, 47, 0, 2, 85, 0, 0, 
        66, 0, 6, 65, 4, 161, 2, 228, 0, 6, 54, 2, 113, 0, 6, 82, 1, 147, 4, 
        172, 5, 4259669018, 2, 85, 0, 0, 131, 1, 2, 118, 0, 6, 96, 3, 0, 0, 
        33, 1, 2, 16, 0, 0, 239, 0, 1, 44, 2, 198, 0, 0, 58, 1, 5, 63944229, 
        6, 40, 3, 1, 1, 129, 2, 129, 1, 4, 4, 6, 0, 5, 1709272325, 0, 180, 0, 
        4, 241, 2, 114, 0, 2, 158, 1, 0, 236, 1, 1, 180, 0, 196, 0, 5, 
        2064179465, 2, 245, 0, 5, 3519718982, 5, 2615211358, 1, 68, 0, 203, 
        1, 5, 3247946259, 3, 0, 1, 57, 3, 1, 1, 198, 1, 107, 4, 157, 2, 29, 
        1, 2, 83, 0, 6, 241, 3, 0, 4, 3, 3, 1, 2, 90, 1, 4, 121, 4, 8, 6, 70, 
        1, 155, 4, 122, 2, 9, 0, 0, 126, 1, 4, 60, 6, 210, 2, 149, 0, 1, 118, 
        1, 91, 4, 14, 6, 97, 6, 82, 6, 63, 6, 46, 5, 736584181, 6, 228, 5, 
        3449578994, 2, 169, 0, 1, 147, 0, 12, 1};
    run_test<ub_suc_bv>(ops);
}

TEST(BBV0, Insertion10) { insert_test<ub_suc_bv>(10); }

TEST(BBV0, Insertion100) { insert_test<ub_suc_bv>(100); }

TEST(BBV0, Insertion1000) { insert_test<ub_suc_bv>(1000); }

TEST(BBV0, Insertion10000) { insert_test<ub_suc_bv>(10000); }

TEST(BBV0, Insertion100000) { insert_test<ub_suc_bv>(100000); }

TEST(BBV0, Insertion1000000) { insert_test<ub_suc_bv>(1000000); }

TEST(BBV0, Mixture10) { mixture_test<ub_suc_bv>(10); }

TEST(BBV0, Mixture100) { mixture_test<ub_suc_bv>(100); }

TEST(BBV0, Mixture1000) { mixture_test<ub_suc_bv>(1000); }

TEST(BBV0, Mixture10000) { mixture_test<ub_suc_bv>(10000); }

TEST(BBV0, Rank10) { rank_test<ub_suc_bv>(10); }

TEST(BBV0, Rank100) { rank_test<ub_suc_bv>(100); }

TEST(BBV0, Rank1000) { rank_test<ub_suc_bv>(1000); }

TEST(BBV0, Rank10000) { rank_test<ub_suc_bv>(10000); }

TEST(BBV0, Rank100000) { rank_test<ub_suc_bv>(100000); }

TEST(BBV0, Rank1000000) { rank_test<ub_suc_bv>(1000000); }

TEST(BBV0, Remove10) { remove_test<ub_suc_bv>(10); }

TEST(BBV0, Remove100) { remove_test<ub_suc_bv>(100); }

TEST(BBV0, Remove1000) { remove_test<ub_suc_bv>(1000); }

TEST(BBV0, Remove10000) { remove_test<ub_suc_bv>(10000); }

TEST(BBV0, Remove100000) { remove_test<ub_suc_bv>(100000); }

TEST(BBV0, Remove1000000) { remove_test<ub_suc_bv>(1000000); }

TEST(BBV0, Update10) { update_test<ub_suc_bv>(10); }

TEST(BBV0, Update100) { update_test<ub_suc_bv>(100); }

TEST(BBV0, Update1000) { update_test<ub_suc_bv>(1000); }

TEST(BBV0, Update10000) { update_test<ub_suc_bv>(10000); }

TEST(BBV0, Update100000) { update_test<ub_suc_bv>(100000); }

TEST(BBV0, Update1000000) { update_test<ub_suc_bv>(1000000); }

TEST(BBV0, Select10) { select_test<ub_suc_bv>(10); }

TEST(BBV0, Select100) { select_test<ub_suc_bv>(100); }

TEST(BBV0, Select1000) { select_test<ub_suc_bv>(1000); }

TEST(BBV0, Select10000) { select_test<ub_suc_bv>(10000); }

TEST(BBV0, Select100000) { select_test<ub_suc_bv>(100000); }

TEST(BBV0, Select1000000) { select_test<b_suc_bv0>(1000000); }

TEST(BBV0, Select0_10) { select0_test<b_suc_bv0>(10); }

TEST(BBV0, Select0_100) { select0_test<b_suc_bv0>(100); }

TEST(BBV0, Select0_1000) { select0_test<b_suc_bv0>(1000); }

TEST(BBV0, Select0_10000) { select0_test<b_suc_bv0>(10000); }

TEST(BBV0, Select0_100000) { select0_test<b_suc_bv0>(100000); }

TEST(BBV0, Select0_1000000) { select0_test<b_suc_bv0>(1000000); }

