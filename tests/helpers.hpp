#pragma once

#include <cstdint>
#include <iostream>

typedef dyn::suc_bv control_bv;

template <class T>
void pv_pushback_test() {
    auto bv = new T();
    for (size_t i = 0; i < 200; i++) {
        bv->push_back(i % 2);
        EXPECT_EQ(bv->psum(), (i + 1) / 2);
    }
    for (size_t i = 0; i < 200; i++) {
        EXPECT_EQ(bv->at(i), i % 2);
    }
    delete bv;
}

template <class T>
void pv_insert_test() {
    auto bv = new T();
    for (size_t i = 0; i < 30; i++) {
        bv->push_back(0);
    }
    auto val = bv->psum();
    EXPECT_EQ(val, 0) << "Psum of pushed back zeros should be 0, was " << val;
    for (size_t i = 0; i < 30; i++) {
        EXPECT_EQ(bv->at(i), 0) << "All entries should be 0. Position " << i;
    }
    for (size_t i = 0; i < 60; i++) {
        bv->insert(15, 1);
        val = bv->psum();
        EXPECT_EQ(val, i + 1) << "Psum after " << i << " insertions should be "
                              << i << " was " << val;
    }

    for (size_t i = 0; i < 90; i++) {
        bool actual = bv->at(i);
        bool expected = i >= 15 && i < 75;
        EXPECT_EQ(actual, expected)
            << "Value at position " << i << " should be " << expected;
    }
    for (size_t i = 0; i < 30; i++) {
        bv->insert(45, 0);
        val = bv->psum();
        EXPECT_EQ(val, 60)
            << "Psum should stay at 60 when inserting zeros. Was " << val
            << " at " << i;
    }

    for (size_t i = 0; i < 120; i++) {
        EXPECT_EQ(bv->at(i), (i >= 15 && i < 45) || (i >= 75 && i < 105))
            << "At position " << i;
    }

    delete bv;
}

template <class T>
void pv_remove_test() {
    auto bv = new T();
    for (size_t i = 0; i < 200; i++) {
        bv->push_back(i % 2 == 1);
    }
    EXPECT_EQ(bv->size(), 200) << "Should have 200 elements pushed";
    EXPECT_EQ(bv->psum(), 100) << "Should have 100 ones pushed";
    for (size_t i = 1; i <= 50; i++) {
        bv->remove(50);
        bv->remove(50);
        EXPECT_EQ(bv->size(), 200 - 2 * i) << "Should have " << (200 - 2 * 1)
                                           << " elements left after removal";
        EXPECT_EQ(bv->psum(), 100 - i)
            << "Should have " << (100 - i) << " ones left after " << (i * 2)
            << " removals";
        for (size_t j = 0; j < (200 - 2 * i); j++) {
            EXPECT_EQ(bv->at(j), 1 == j % 2)
                << "Alernating pattern should hold after " << i
                << " removals at pos " << j;
        }
    }

    for (size_t i = 1; i <= 50; i++) {
        bv->remove(0);
        for (size_t j = 0; j < (100 - (2 * i) + 1); j++) {
            EXPECT_EQ(bv->at(j), 1 != j % 2)
                << "Second removals: Alernating pattern should hold after " << i
                << " removals at pos " << j;
        }
        bv->remove(0);
        for (size_t j = 0; j < (100 - i * 2); j++) {
            EXPECT_EQ(bv->at(j), 1 == j % 2)
                << "Second removals 2: Alernating pattern should hold after "
                << i << " removals at pos " << j;
        }
    }
    EXPECT_EQ(bv->size(), 0) << "Should be no elements left";
    EXPECT_EQ(bv->psum(), 0) << "Should be no ones left";
}

template <class T>
T* generate_tree(const uint64_t amount) {
    auto tree = new T();

    for (uint64_t i = 0; i < amount; i++) {
        tree->push_back(i % 2);
    }

    for (uint64_t i = 0; i < amount; i++) {
        EXPECT_EQ(tree->at(i), i % 2);
    }

    return tree;
}

template <class T>
void run_test(std::vector<uint32_t>& ops) {
    auto buffered_tree = generate_tree<T>(ops[0]);
    auto control_tree = generate_tree<control_bv>(ops[0]);

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
                ASSERT_EQ(ic_r, ops[i + 1] % buffered_tree->size())
                    << "Unexpected tree size. " << ops[i + 1]
                    << " % tree_size should be " << ic_r;
                uint64_t b_r = buffered_tree->rank(ic_r);
                uint64_t c_r = control_tree->rank(ic_r);
                ASSERT_EQ(b_r, c_r)
                    << "Unexpected result of rank(" << ic_r
                    << ") query at " << i <<". Expected " << c_r << ", got " << b_r;
                } 
                break;
            default:
                s = buffered_tree->size();
                step = 2;
                if (s == 0) break;
                uint64_t ba_r = buffered_tree->rank(buffered_tree->size());
                uint64_t ca_r = control_tree->rank(control_tree->size());
                ASSERT_EQ(ba_r, ca_r)
                    << "Unexpected one count for rank query. Expected " << ca_r
                    << ", got " << ba_r;
                if (ba_r == 0) break;
                auto loc = ops[i + 1] % ba_r;
                uint64_t b_s = buffered_tree->select(loc);
                uint64_t c_s = control_tree->select(loc);
                ASSERT_EQ(b_s, c_s)
                    << "Unexpected result of select(" << loc
                    << ") query. Expected " << c_s << ", got " << b_s;
                break;
        }
        uint64_t size = control_tree->size();
        ASSERT_EQ(size, buffered_tree->size())
            << "Expected vector size of " << size << " but found "
            << buffered_tree->size();
        for (size_t idx = 0; idx < size; idx++) {
            ASSERT_EQ(control_tree->at(idx), buffered_tree->at(idx))
                << "Expected " << control_tree->at(idx) << " at position "
                << idx << " after op " << i;
        }
        i += step;
    }
    delete buffered_tree;
    delete control_tree;
}

template <class T>
void insert_test(const uint64_t size) {
    assert((size & uint64_t(1)) && "input size should be even");
    auto tree = generate_tree<T>(0);
    for (uint64_t i = 0; i < size; i++) {
        auto set = i % 2;
        tree->insert(i, set);
        auto val = tree->at(i);
        EXPECT_EQ(val, set)
            << "Expected insert appended value of " << i % 2 << " at " << i;
        auto s = tree->size();
        EXPECT_EQ(s, i + 1) << "Expected insert appended tree size of " << i + 1
                            << " got " << s;
    }

    for (uint64_t i = 0; i < size; i++) {
        auto set = i % 2 == 1;
        tree->insert(i, set);
        auto val = tree->at(i);
        EXPECT_EQ(val, set) << "Value at " << i << " should be set to " << set;
        auto s = tree->size();
        EXPECT_EQ(s, size + i + 1)
            << "Tree should have " << size + i + 1 << " elements";
    }

    for (uint64_t i = 0; i < size << 1; i++) {
        EXPECT_EQ(tree->at(i), i % 2)
            << "Value at " << i << " should be " << i % 2;
    }

    delete tree;
}

template <class T>
void mixture_test(const uint64_t size) {
    auto tree = generate_tree<T>(0);

    for (uint64_t i = 0; i < size; i++) {
        auto set = i % 2;
        tree->insert(i, set);
        auto val = tree->at(i);
        EXPECT_EQ(val, set) << "Value after insert should be " << (1 % 2);
        if (val != set) {
            break;
        }
        auto s = tree->size();
        EXPECT_EQ(s, i + 1) << "Size after insert should be " << (i + 1);
        if (s != i + 1) {
            return;
        }
    }

    for (uint64_t i = 0; i < size; i++) {
        auto val = tree->at(i);
        EXPECT_EQ(val, i % 2)
            << "Alternating patterns should hold. Problem at " << i;
        if (val != i % 2) return;
    }

    for (uint64_t i = 1; i < size; i++) {
        tree->remove(0);
        auto new_size = tree->size();
        EXPECT_EQ(size - i, new_size)
            << "Expect size of " << (size - i) << " after removal";
        if (size - i != new_size) {
            return;
        }
        bool prev = tree->at(0);
        for (uint64_t j = 1; j < new_size; j++) {
            bool val = tree->at(j);
            EXPECT_NE(val, prev) << "Alternating pattern should hold after "
                                 << i << " removals, at position " << j;
            prev = val;
        }
    }

    delete tree;
}

template <class T>
void update_test(const uint64_t size) {
    auto tree = generate_tree<T>(size);
    for (uint64_t i = 0; i < size; i++) {
        bool set = (i + 1) % 2;
        tree->set(i, set);
        auto val = tree->at(i);
        EXPECT_EQ(val, set);
        if (val != set) {
            break;
        }
    }
    delete tree;
}

template <class T>
void rank_test(const uint64_t size) {
    auto tree = generate_tree<T>(size);
    uint64_t sum = 0;
    for (uint64_t i = 1; i <= size; i++) {
        auto val = tree->rank(i);
        sum += tree->at(i - 1);
        EXPECT_EQ(sum, val);
        if (sum != val) {
            break;
        }
    }
    delete tree;
}

template <class T>
void select_test(const uint64_t size) {
    auto tree = generate_tree<T>(size);
    for (uint64_t i = 0; i < size / 2; i++) {
        auto val = tree->select(i);
        EXPECT_EQ(i * 2 + 1, val)
            << "Expected select(" << i << ") = " << (i * 2 + 1);
        if ((i - 1) * 2 != val) {
            break;
        }
    }
    delete tree;
}

template <class T>
void remove_test(const uint64_t size) {
    auto tree = generate_tree<T>(size);
    EXPECT_EQ(size, tree->size()) << "Initial tree size needs to be " << size;
    for (uint64_t i = 0; i < size - 1; i++) {
        auto old = tree->at(0);
        tree->remove(0);
        auto new_size = tree->size();
        EXPECT_TRUE(size - i - 1 == new_size)
            << "Tree size after removal should be " << size - i - 1;
        if (i + 1 != new_size) {
            break;
        }
        auto expected = !old;
        auto val = tree->at(0);
        EXPECT_EQ(val, expected)
            << "Alternating pattern should hold on removal. Iteration: " << i;
        if (val != expected) {
            break;
        }
    }
    delete tree;
}