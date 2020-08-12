
#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

template <class T>
uint8_t execute_op(T &buffered_tree, std::vector<uint32_t> &ops, size_t i,
                   uint64_t &out) {
    uint32_t selection = ops[i];
    uint64_t s = buffered_tree.size();
    switch (selection) {
        case 0:
            buffered_tree.insert(s ? ops[i + 1] % s : 0, ops[i + 2]);
            out = 0;
            return 3;
        case 1:
            out = 0;
            if (s == 0) return 2;
            buffered_tree.remove(ops[i + 1] % s);
            return 2;
        case 2:
            out = 0;
            if (s == 0) return 3;
            buffered_tree.set(ops[i + 1] % s, ops[i + 2]);
            return 3;
        case 3:
            buffered_tree.push_back(ops[i + 1]);
            out = 0;
            return 2;
        case 4:
            out = 0;
            if (s == 0) return 2;
            out = buffered_tree.rank(ops[i + 1] % s);
            return 2;
        case 5: {
            out = 0;
            if (s == 0) return 2;
            uint64_t ba_r = buffered_tree.rank(buffered_tree.size() - 1);
            if (ba_r == 0) return 2;
            out = buffered_tree.select(ops[i + 1] % ba_r);
            }
            return 2;
        default:
            out = 0;
            if (s == 0) return 2;
            out = buffered_tree.at(ops[i + 1] % s);
            return 2;
    }
}

template <class A, class B>
void output_comparison(A &a_tree, B &b_tree) {
    auto as = a_tree->size();
    auto bs = b_tree->size();
    size_t s_idx = 0;
    while (s_idx < as || s_idx < bs) {
        std::cout << " pos " << s_idx << ":\n ";
        for (size_t idx = s_idx; idx < s_idx + 64; idx++) {
            if (idx >= as || idx >= bs) break;
            std::cout << (a_tree->at(idx) ? 1 : 0);
        }
        std::cout << "\n ";
        for (size_t idx = s_idx; idx < s_idx + 64; idx++) {
            if (idx >= b_tree->size()) break;
            std::cout << (b_tree->at(idx) == a_tree->at(idx)
                              ? ""
                              : "\033[31m")
                      << (b_tree->at(idx) ? 1 : 0) << "\033[0m";
        }
        std::cout << std::endl;
        s_idx += 64;
    }
}

template <class T>
double run_timing(std::vector<uint32_t> &ops) {
    
    auto tree = new T();

    for (size_t i = 0; i < ops[0]; i++) {
        tree->push_back(i % 2);
    }

    auto start = std::chrono::steady_clock::now();
    size_t i = 1;
    uint64_t tot = 0;
    uint64_t val = 0;
    while (i < ops.size()) {
        uint8_t res = execute_op<T>(*tree, ops, i, val);
        i += res;
        tot += val;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end-start;
    std::cerr << "tot: " << tot << std::endl;

    delete tree;

    return elapsed.count();
}

template <class A, class B>
bool run_test(std::vector<uint32_t> &ops) {
    auto a_tree = new A();
    auto b_tree = new B();

    for (size_t i = 0; i < ops[0]; i++) {
        a_tree->push_back(i % 2);
        b_tree->push_back(i % 2);
    }
    
    size_t i = 1;
    uint64_t val_a = 0;
    uint64_t val_b = 0;

    while (i < ops.size()) {
        uint8_t res = execute_op<A>(*a_tree, ops, i, val_a);
        uint8_t res_c = execute_op<B>(*b_tree, ops, i, val_b);
        if (res_c != res) {
            std::cerr << "Invalid op counter encountered at " << i << std::endl;
            return true;
        }
        if (val_a != val_b) {
            std::cerr << "Invalid return value at " << i << std::endl;
            return true;
        }
        i += res;
    }

    delete a_tree;
    delete b_tree;
    return false;
}

template <class T, class C>
bool test(std::vector<uint32_t> &ops) {
    auto comp_tree = new T();
    auto control = new C();

    for (size_t i = 0; i < ops[0]; i++) {
        comp_tree->push_back(i % 2);
        control->push_back(i % 2);
    }
    if (comp_tree->size() != control->size() || control->size() != ops[0]) {
        std::cout << "Expected vector size of " << ops[0] << " found " << comp_tree->size() << " and " << control->size() << std::endl;
        return false;
    }
    for (size_t i = 0; i < ops[0]; i++) {
        if (comp_tree->at(i) != control->at(i)) {
            std::cout << "Problem at " << i << " after seeding" << std::endl;
            output_comparison(control, comp_tree);
            return false;
        }
    }

    uint64_t i = 1;
    uint64_t val_a = 0;
    uint64_t val_b = 0;

    while (i < ops.size()) {
        uint8_t res = execute_op<C>(*control, ops, i, val_a);
        uint8_t res_c = execute_op<T>(*comp_tree, ops, i, val_b);
        if (res_c != res) {
            std::cout << "Invalid op counter encountered at " << i << std::endl;
            output_comparison(control, comp_tree);
            return false;
        }
        if (val_a != val_b) {
            std::cout << "Invalid return value at " << i << std::endl;
            output_comparison(control, comp_tree);
            return false;
        }
        for (size_t j = 0; j < control->size(); j++) {
            if (control->at(j) != comp_tree->at(j)) {
                std::cout << "Incompatible values after op " << i << std::endl;
                output_comparison(control, comp_tree);
                return false;
            }
        }
        i += res;
    }

    delete comp_tree;
    delete control;
    return false;
}