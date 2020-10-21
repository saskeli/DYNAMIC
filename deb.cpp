#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include "include/dynamic/dynamic.hpp"

#include "runners.hpp"

int8_t get_op(std::vector<uint32_t> &ops, std::mt19937 &gen, uint32_t size) {
    uint32_t selection = gen() % 7;
    ops.push_back(selection);
    switch (selection) {
        case 0:
            ops.push_back(size ? gen() % size : 0);
            ops.push_back(gen() % 2);
            return 1;
        case 1:
            ops.push_back(size ? gen() % size : 0);
            return -1;
        case 2:
            ops.push_back(size ? gen() % size : 0);
            ops.push_back(gen() % 2);
            return 0;
        case 3:
            ops.push_back(gen() % 2);
            return 1;
        case 4:
            ops.push_back(size ? gen() % size : 0);
            return 0;
        case 5:
            ops.push_back(gen());
            return 0;
        default:
            ops.push_back(size ? gen() % size: 0);
            return 0;
    }
}

int main(int argc, char **argv) {

    std::vector<uint32_t> ops;
    
    if (argc > 1) {
        for (size_t i = 1; i < argc; i++) {
            ops.push_back(atoi(argv[i]));
        }
        std::cout << "Read " << ops.size() << " operations" << std::endl;
        bool res = true;
        if (string(argv[1]).compare("-u") == 0) {
            res = test<dyn::ub_suc_bv, dyn::suc_bv>(ops);
        } else {
            res = test<dyn::b_suc_bv, dyn::suc_bv>(ops);
        }
        /*
        if (!res) {
            for (size_t i = 0; i < ops.size(); i++) {
                std::cout << i << ": " << ops[i] << ", ";
            }
            std::cout << std::endl;
        }*/
        return 0;
    }

    int w = 12;

    std::random_device rd;
    std::mt19937 gen(rd());

    uint32_t initial_size_limit = 100000;
    uint32_t num_ops = 10000;
    
    std::cout << "control\tbuffered\tunbuffered" << std::endl;
    for (size_t i = 0; i < 100000; i++) {
        ops.clear();
        uint16_t size = gen() % initial_size_limit;
        ops.push_back(size);
        for (size_t i = 0; i < num_ops; i++) {
            size += get_op(ops, gen, size);
        }
        std::cerr << "input: ";
        for (auto v : ops) std::cerr << v << " ";
        std::cerr << std::endl;
        if (run_test<dyn::suc_bv, dyn::b_suc_bv>(ops)) {
            std::cerr << "Problem between control and buffered vectors" << std::endl;
            break;
        }
        std::cout << std::setw(w) << run_timing<dyn::suc_bv>(ops) << std::flush;
        if (run_test<dyn::b_suc_bv, dyn::ub_suc_bv>(ops)) {
            std::cerr << "Problem between buffered and unbuffered vectors" << std::endl;
            break;
        }
        std::cout << std::setw(w) << run_timing<dyn::b_suc_bv>(ops) << std::flush;
        if (run_test<dyn::ub_suc_bv, dyn::suc_bv>(ops)) {
            std::cerr << "Problem between unbuffered and control vectors" << std::endl;
            break;
        }
        std::cout << std::setw(w) << run_timing<dyn::ub_suc_bv>(ops) << std::flush;
        
        std::cout << std::endl;
    }
    return 0;
}
