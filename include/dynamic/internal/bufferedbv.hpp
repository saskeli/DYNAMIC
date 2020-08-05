// Copyright (c) 2017, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

namespace dyn {
template <uint8_t buffer_size>
class buffered_packed_bit_vector {
   public:
    static uint64_t fast_mod(uint64_t const num) { return num & 63; }

    static uint64_t fast_div(uint64_t const num) { return num >> 6; }

    static uint64_t fast_mul(uint64_t const num) { return num << 6; }

    explicit buffered_packed_bit_vector(uint64_t const size = 0) {
        assert(buffer_size >= 1 && buffer_size <= 64);
        std::fill(buffer, buffer + buffer_size, 0);
        buffer_count = 0;
        this->size_ = size;
        this->psum_ = 0;

        words = std::vector<uint64_t>(fast_div(size_) + (fast_mod(size_) != 0));
        assert(size_ / int_per_word_ <= words.size());
        assert((size_ / int_per_word_ == words.size() ||
                !(words[size_ / int_per_word_] >>
                  ((size_ % int_per_word_) * width_))) &&
               "uninitialized non-zero values in the end of the vector");
    }

    explicit buffered_packed_bit_vector(std::vector<uint64_t>&& _words,
                                    uint64_t const new_size) {
        assert(buffer_size > 1 && buffer_size <= 64);

        std::fill(buffer, buffer + buffer_size, 0);
        buffer_count = 0;

        this->words = std::move(_words);
        this->size_ = new_size;
        this->psum_ = psum(size_ - 1);

        assert(size_ / int_per_word_ <= words.size());
        assert((size_ / int_per_word_ == words.size() ||
                !(words[size_ / int_per_word_] >>
                  ((size_ % int_per_word_) * width_))) &&
               "uninitialized non-zero values in the end of the vector");
    }

    ~buffered_packed_bit_vector() = default;

    void print() const {
        std::cout << "Leaf: " << size_ << " elems and " << psum_ << " ones";
        for (size_t i = 0; i < buffer_count; i++) {
            std::cout << "\n " << buffer_index(buffer[i]) << ", "
                      << buffer_is_insertion(buffer[i]) << ", "
                      << buffer_value(buffer[i]);
        }
        for (auto w : words) {
            std::cout << "\n ";
            for (size_t k = 0; k < 64; k++) {
                std::cout << ((w & (MASK << k)) ? 1 : 0);
            }
        }
        std::cout << std::endl;
    }

    bool at(uint64_t i) const {
        assert(i < size());

        uint64_t index = i;
        for (uint8_t idx = 0; idx < buffer_count; idx++) {
            uint64_t b = buffer_index(buffer[idx]);
            if (b == i) {
                if (buffer_is_insertion(buffer[idx])) {
                    return buffer_value(buffer[idx]);
                }
                index++;
            } else if (b < i) {
                index += buffer_is_insertion(buffer[idx]) ? -1 : 1;
            } else {
                break;
            }
        }
        return MASK & (words[fast_div(index)] >> fast_mod(index));
    }

    uint64_t psum() const { return psum_; }

    /*
     * inclusive partial sum (i.e. up to element i included)
     */
    uint64_t psum(uint64_t i) const { 
        assert(i < size_);
        return rank(i + 1); 
    }

    /*
     * smallest index j such that psum(j)>=x
     */
    uint64_t search(uint64_t x) const {
        assert(size_ > 0);
        assert(x <= psum_);

        uint64_t pop = 0;
        uint64_t pos = 0;
        uint8_t current_buffer = 0;
        int8_t a_pos_offset = 0;

        // optimization for bitvectors

        for (uint64_t j = 0; j < words.size(); ++j) {
            pop += __builtin_popcountll(words[j]);
            pos += 64;
            for (uint8_t b = current_buffer; b < buffer_count; b++) {
                uint32_t b_index = buffer_index(buffer[b]);
                if (b_index < pos) {
                    if (buffer_is_insertion(buffer[b])) {
                        pop += buffer_value(buffer[b]);
                        pos++;
                        a_pos_offset--;
                    } else {
                        pop -= (words[fast_div(b_index + a_pos_offset)] &
                                (MASK << fast_mod(b_index + a_pos_offset)))
                                   ? 1
                                   : 0;
                        pos--;
                        a_pos_offset++;
                    }
                    current_buffer++;
                } else {
                    break;
                }
            }
            if (pop >= x) break;
        }
        pos = size_ < pos ? size_ : pos;
        // end optimization for bitvectors
        while (pop >= x) {
            pop -= at(--pos);
        }
        return pos;
    }

    /*
     * this function works only for bitvectors, and
     * is designed to support select_0. Returns first
     * position i such that the number of zeros before
     * i (included) is == x
     */
    uint64_t search_0(uint64_t x) const {
        // TODO: TESTS
        assert(size_ > 0);
        assert(width_ == 1);
        assert(x <= size_ - psum_);

        uint64_t s = 0;
        uint64_t pop = 0;
        uint64_t pos = 0;
        uint8_t current_buffer = 0;
        int8_t a_pos_offset = 0;

        auto div = fast_div(size_);
        for (uint64_t j = 0; j < div && s < x; ++j) {
            pop = 64 - __builtin_popcountll(words[j]);
            pos += 64;
            s += pop;
            for (uint8_t b = current_buffer; b < buffer_count; b++) {
                uint32_t b_index = buffer_index(buffer[b]);
                if (b_index < pos) {
                    if (buffer_is_insertion(buffer[b])) {
                        pop += buffer_value(buffer[b]) ? 0 : 1;
                        pos++;
                        a_pos_offset--;
                    } else {
                        pop -= (words[fast_div(b_index + a_pos_offset)] &
                                (MASK << fast_mod(b_index + a_pos_offset)))
                                   ? 0
                                   : 1;
                        pos--;
                        a_pos_offset++;
                    }
                    current_buffer++;
                } else {
                    break;
                }
            }
            if (pop >= x) break;
        }

        pos = size_ < pos ? size_ : pos;

        while (pop >= x) {
            pop -= at(--pos) ? 0 : 1;
        }
        return pos;
    }

    /*
     * smallest index j such that psum(j)+j>=x
     */
    uint64_t search_r(uint64_t x) const {
        // FIXME: Make buffered
        assert(size_ > 0);
        assert(x <= psum_ + size_);

        uint64_t s = 0;
        uint64_t pop = 0;
        uint64_t pos = 0;

        auto div = fast_div(size_);
        for (uint64_t j = 0; j < div && s < x; ++j) {
            pop = uint64_t(64) + __builtin_popcountll(words[j]);
            pos += 64;
            s += pop;
        }

        pos -= fast_mul(pos > 0);
        s -= pop;

        for (; pos < size_ && s < x; ++pos) {
            s += (uint64_t(1) + at(pos));
        }

        pos -= pos != 0;
        return pos;
    }

    /*
     * true iif x is one of the partial sums  0, I_0, I_0+I_1, ...
     */
    bool contains(uint64_t x) const {
        assert(size_ > 0);
        assert(x <= psum_);

        uint64_t s = 0;

        for (uint64_t j = 0; j < size_ && s < x; ++j) {
            s += at(j);
        }

        return s == x;
    }

    /*
     * true iif x is one of  0, I_0+1, I_0+I_1+2, ...
     */
    bool contains_r(uint64_t x) const {
        assert(size_ > 0);
        assert(x <= psum_ + size_);

        uint64_t s = 0;

        for (uint64_t j = 0; j < size_ && s < x; ++j) {
            s += (at(j) + uint64_t(1));
        }

        return s == x;
    }

    void increment(uint64_t i, bool delta, bool subtract = false) {
        assert(i < size_);

        auto pvi = at(i);

        if (subtract) {
            set_without_psum_update(i, pvi - delta);
            psum_ -= delta;
        } else {
            uint64_t s = pvi + delta;
            psum_ += delta;
            set_without_psum_update(i, s);
        }
    }

    void append(uint64_t x) { push_back(x); }

    void remove(uint64_t i) {
        assert(i < size_);
        auto x = this->at(i);
        psum_ -= x;
        --size_;
        bool done = false;
        for (uint8_t idx = 0; idx < buffer_count; idx++) {
            uint32_t b = buffer_index(buffer[idx]);
            if (b == i && buffer_is_insertion(buffer[idx]) && !done) {
                delete_buffer_element(idx--);
                done = true;
                continue;
            }
            if (b > i) {
                if (!done) {
                    insert_buffer(idx, create_buffer(i, 0, x));
                    done = true;
                } else {
                    set_buffer_index(b - 1, idx);
                }
            }
        }
        if (!done) {
            buffer[buffer_count] = create_buffer(i, 0, x);
            buffer_count++;
        }
        if (buffer_count == buffer_size) commit();
    }

    void insert(uint64_t i, uint64_t x) {
        if (i == size_) {
            push_back(x);
            return;
        }
        psum_ += x ? 1 : 0;
        bool done = false;
        int a_pos = 0;
        for (uint8_t idx = 0; idx < buffer_count; idx++) {
            uint32_t b = buffer_index(buffer[idx]);
            if (b < i) {
                a_pos += buffer_is_insertion(buffer[idx]) ? -1 : 1;
            // If this buffered remove can be used to turn the insert into a set operation:
            } else if (b == i && !done && !buffer_is_insertion(buffer[idx]) &&
                       buffer_value(buffer[idx]) ==
                           ((words[fast_div(b + a_pos)] &
                             (MASK << fast_mod(b + a_pos)))
                                ? true
                                : false)) {
                delete_buffer_element(idx--);
                done = true;
                a_pos += b;
                const auto word_nr = fast_div(a_pos);
                const auto pos = fast_mod(a_pos);
                words[word_nr] ^=
                    (words[word_nr] & (MASK << pos)) != (uint64_t(x) << pos)
                        ? (MASK << pos)
                        : 0;
            } else {
                if (!done) {
                    insert_buffer(idx, create_buffer(i, 1, x));
                    done = true;
                } else {
                    set_buffer_index(b + 1, idx);
                }
            }
        }
        size_++;
        if (!done) {
            buffer[buffer_count] = create_buffer(i, 1, x);
            buffer_count++;
        }
        if (buffer_count == buffer_size) commit();
    }

    /*
     * efficient push-back, implemented with a push-back on the underlying
     * container the insertion of an element whose bit-size exceeds the current
     * width causes a rebuild of the whole vector!
     */
    void push_back(uint64_t x) {
        auto pb_size = size_;
        for (uint8_t i = 0; i < buffer_count; i++) {
            pb_size += buffer_is_insertion(buffer[i]) ? -1 : 1;
        }
        size_++;
        assert(int_per_word_ == 64);
        assert(pb_size <= words.size() * 64);

        // not enough space for the new element:
        // push back a new word
        if (fast_div(pb_size) == words.size()) words.push_back(0);

        if (x) {
            // insert x at the last position
            words[fast_div(pb_size)] |= MASK << fast_mod(pb_size);
            psum_++;
        }

        assert((((size_ << 1) - pb_size) >> 6) <= words.size());
    }

    uint64_t size() const { return size_; }

    /*
     * split content of this vector into 2 packed blocks:
     * Left part remains in this block, right part in the
     * new returned block
     */
    buffered_packed_bit_vector* split() {
        if (buffer_count > 0) {
            commit();
        }

        uint64_t tot_words = fast_div(size_) + (fast_mod(size_) != 0);

        assert(tot_words <= words.size());

        uint64_t nr_left_words = tot_words >> 1;

        assert(nr_left_words > 0);
        assert(tot_words - nr_left_words > 0);

        uint64_t nr_left_ints = fast_mul(nr_left_words);

        assert(size_ > nr_left_ints);
        uint64_t nr_right_ints = size_ - nr_left_ints;

        assert(words.begin() + nr_left_words + extra_ < words.end());
        assert(words.begin() + tot_words <= words.end());
        std::vector<uint64_t> right_words(tot_words - nr_left_words + extra_,
                                          0);
        std::copy(words.begin() + nr_left_words, words.begin() + tot_words,
                  right_words.begin());
        words.resize(nr_left_words + extra_);
        std::fill(words.begin() + nr_left_words, words.end(), 0);
        words.shrink_to_fit();

        size_ = nr_left_ints;
        psum_ = psum(size_ - 1);

        auto right =
            new buffered_packed_bit_vector(std::move(right_words), nr_right_ints);

        assert(size_ / int_per_word_ <= words.size());
        assert((size_ / int_per_word_ == words.size() ||
                !(words[size_ / int_per_word_] >>
                  ((size_ % int_per_word_) * width_))) &&
               "uninitialized non-zero values in the end of the vector");

        return right;
    }

    /* set i-th element to x. updates psum */
    void set(const uint64_t i, const bool x) {
        uint64_t idx = i;
        for (uint8_t j = 0; j < buffer_count; j++) {
            uint32_t b = buffer_index(buffer[j]);
            if (b < i) {
                idx += buffer_is_insertion(buffer[j]) ? -1 : 1;
            } else if (b == i) {
                if (buffer_is_insertion(buffer[j])) {
                    if (buffer_value(buffer[j]) != x) {
                        psum_ += x ? 1 : -1;
                        buffer[j] ^= VALUE_MASK;
                    }
                    return;
                }
                idx++;
            } else {
                break;
            }
        }
        const auto word_nr = fast_div(idx);
        const auto pos = fast_mod(idx);

        if ((words[word_nr] & (MASK << pos)) != (uint64_t(x) << pos)) {
            psum_ += x ? 1 : -1;
            words[word_nr] ^= MASK << pos;
        }
    }

    /*
     * return total number of bits occupied in memory by this object instance
     */
    uint64_t bit_size() const {
        return (sizeof(buffered_packed_bit_vector) +
                words.capacity() * sizeof(uint64_t) +
                sizeof(buffer) * sizeof(uint32_t) + 1) *
               8;
    }

    uint64_t width() const { return width_; }

    void insert_word(uint64_t i, uint64_t word, uint8_t width, uint8_t n) {
        // TODO: Test?
        assert(false && "No proper implementation");
        assert(i <= size());
        assert(n);
        assert(n * width <= sizeof(word) * 8);
        assert(width * n == 64 || (word >> width * n) == 0);

        if (buffer_count > 0) commit();

        if (n == 1) {
            // only one integer to insert
            insert(i, word);

        } else if (width == 1 && width_ == 1 && n == 64) {
            // insert 64 bits packed into a word
            uint64_t pos = size_ / 64;
            uint8_t offset = size_ - pos * 64;

            if (!offset) {
                words.insert(words.begin() + pos, word);
            } else {
                assert(pos + 1 < words.size());

                words.insert(words.begin() + pos, words[pos + 1]);

                words[pos] &= ((1llu << offset) - 1);
                words[pos] |= word << offset;

                words[pos + 1] &= ~((1llu << offset) - 1);
                words[pos + 1] &= word >> (64 - offset);
            }

            size_ += n;
            psum_ += __builtin_popcountll(word);

        } else {
            const uint64_t mask = (1llu << width) - 1;
            while (n--) {
                insert(i++, word & mask);
                word >>= width;
            }
        }
    }

    uint64_t rank(uint64_t n) const {
        uint64_t count = 0;

        uint64_t idx = n;
        for (uint8_t i = 0; i < buffer_count; i++) {
            if (buffer_index(buffer[i]) >= n) break;
            if (buffer_is_insertion(buffer[i])) {
                idx--;
                count += buffer_value(buffer[i]);
            } else {
                idx++;
                count -= buffer_value(buffer[i]);
            }
        }

        uint64_t target_word = fast_div(idx);
        uint64_t target_offset = fast_mod(idx);
        for (size_t i = 0; i < target_word; i++) {
            count += __builtin_popcountll(words[i]);
        }
        count += __builtin_popcountll(words[target_word] &
                                      ((MASK << target_offset) - 1));
        return count;
    }

    uint64_t select(uint64_t n) { return search(n + 1); }

   private:
    bool buffer_value(uint32_t e) const { return (e & VALUE_MASK) != 0; }

    bool buffer_is_insertion(uint32_t e) const { return (e & TYPE_MASK) != 0; }

    uint32_t buffer_index(uint32_t e) const { return (e & INDEX_MASK) >> 8; }

    void set_buffer_index(uint32_t v, uint8_t i) {
        buffer[i] = (v << 8) | (buffer[i] & ((MASK << 7) - 1));
    }

    uint32_t create_buffer(uint32_t idx, bool t, bool v) {
        return ((idx << 8) | (t ? TYPE_MASK : uint32_t(0))) |
               (v ? VALUE_MASK : uint32_t(0));
    }

    void insert_buffer(uint8_t idx, uint32_t buf) {
        for (uint8_t i = buffer_count; i > idx; i--) {
            buffer[i] = buffer[i - 1];
        }
        buffer[idx] = buf;
        buffer_count++;
    }

    void delete_buffer_element(uint8_t idx) {
        uint8_t l = --buffer_count;
        for (; idx < l; idx++) {
            buffer[idx] = buffer[idx + 1];
        }
        buffer[l] = 0;
    }

    void set_without_psum_update(uint64_t i, uint64_t x) {
        uint64_t idx = i;
        for (uint8_t j = 0; j < buffer_count; j++) {
            uint32_t b = buffer_index(buffer[j]);
            if (b < i) {
                idx += buffer_is_insertion(buffer[j]) ? -1 : 1;
            } else if (b == i) {
                if (buffer_is_insertion(buffer[j])) {
                    if (buffer_value(buffer[j]) != x) {
                        buffer[j] ^= VALUE_MASK;
                    }
                    return;
                }
                idx++;
            }
        }
        const auto word_nr = fast_div(idx);
        const auto pos = fast_mod(idx);

        if ((words[word_nr] & (MASK << pos)) != (uint64_t(x) << pos)) {
            words[word_nr] ^= MASK << pos;
        }
    }

    void commit() {
        if (size_ > fast_mul(words.size())) {
            words.reserve(words.size() + extra_);
            words.resize(words.size() + extra_, 0);
        }

        uint64_t overflow = 0;
        uint8_t overflow_length = 0;
        uint8_t underflow_length = 0;
        size_t current_word = 0;
        uint8_t current_index = 0;
        uint32_t buf = buffer[current_index];
        size_t target_word = fast_div(buffer_index(buf));
        size_t target_offset = fast_mod(buffer_index(buf));

        while (current_word < words.size()) {
            uint64_t underflow =
                current_word + 1 < words.size() ? words[current_word + 1] : 0;
            if (overflow_length) {
                underflow = (underflow << overflow_length) |
                            (words[current_word] >> (64 - overflow_length));
            }

            uint64_t new_overflow = 0;
            //If buffers need to be commit to this word:
            if (current_word == target_word && current_index < buffer_count) {
                uint64_t word =
                    underflow_length
                        ? (words[current_word] >> underflow_length) |
                              (underflow << (64 - underflow_length))
                        : (words[current_word] << overflow_length) | overflow;
                underflow >>= underflow_length;
                uint64_t new_word = 0;
                uint8_t start_offset = 0;
                //While there are buffers for this word
                while (current_word == target_word) {
                    new_word |=
                        (word << start_offset) & ((MASK << target_offset) - 1);
                    word = (word >> (target_offset - start_offset)) |
                           (target_offset == 0
                                ? 0
                                : target_offset - start_offset == 0
                                      ? 0
                                      : (underflow << (64 - (target_offset -
                                                             start_offset))));
                    underflow >>= target_offset - start_offset;
                    if (buffer_is_insertion(buf)) {
                        if (buffer_value(buf)) {
                            new_word |= MASK << target_offset;
                        }
                        start_offset = target_offset + 1;
                        if (underflow_length)
                            underflow_length--;
                        else
                            overflow_length++;
                    } else {
                        word >>= 1;
                        word |= underflow << 63;
                        underflow >>= 1;
                        if (overflow_length)
                            overflow_length--;
                        else
                            underflow_length++;
                        start_offset = target_offset;
                    }
                    current_index++;
                    if (current_index >= buffer_count) break;
                    buf = buffer[current_index];
                    target_word = fast_div(buffer_index(buf));
                    target_offset = fast_mod(buffer_index(buf));
                }
                new_word |=
                    start_offset < 64 ? (word << start_offset) : uint64_t(0);
                new_overflow = overflow_length ? words[current_word] >>
                                                     (64 - overflow_length)
                                               : 0;
                words[current_word] = new_word;
            } else {
                if (underflow_length) {
                    words[current_word] =
                        (words[current_word] >> underflow_length) |
                        (underflow << (64 - underflow_length));
                } else if (overflow_length) {
                    new_overflow =
                        words[current_word] >> (64 - overflow_length);
                    words[current_word] =
                        (words[current_word] << overflow_length) | overflow;
                    overflow = new_overflow;
                } else {
                    overflow = 0;
                }
            }
            overflow = new_overflow;
            current_word++;
        }
        buffer_count = 0;
    }

    void shift_right(uint64_t i, uint64_t current_word) {
        // TODO: Test
        assert(i < size());
        // number of integers that fit in a memory word
        assert(int_per_word_ > 0);
        
        if (buffer_count > 0) commit();

        assert(size_ + 1 <= fast_mul(words.size()));

        uint64_t index = fast_mod(i);

        // integer that falls out from the right of current word
        uint64_t falling_out = 0;

        auto val = fast_mul(current_word);
        if (val < i) {
            falling_out =
                (words[current_word] >> (int_per_word_ - 1) * width_) &
                uint64_t(1);
            uint64_t word = words[current_word];
            uint64_t one_mask = (uint64_t(1) << index) - 1;
            uint64_t zero_mask = ~one_mask;
            uint64_t unchanged = word & one_mask;
            word <<= 1;
            words[current_word] = (word & zero_mask) | unchanged;
            current_word++;
        }

        // now for the remaining integers we can work blockwise

        uint64_t falling_out_temp;

        // val = fast_div(size_);
        const auto s = words.size();
        for (uint64_t j = current_word; j < s; ++j) {
            assert(j < words.size());

            falling_out_temp = (words[j] >> (int_per_word_ - 1)) & uint64_t(1);

            words[j] = (words[j] << 1) | falling_out;

            falling_out = falling_out_temp;
        }
    }

    // shift left of 1 position elements starting
    // from the (i + 1)-st.
    void shift_left(const uint64_t i) {
        // TODO: Test?
        // number of integers that fit in a memory word
        assert(int_per_word_ > 0);
        assert(i < size_);

        if (buffer_count > 0) commit();

        if (i == (size_ - 1)) {
            set(i, false);
            return;
        }

        uint64_t current_word = fast_div(i);

        // integer that falls in from the right of current word
        uint64_t falling_in_idx;

        if (fast_mul(current_word) < i) {
            falling_in_idx = std::min(fast_mul(current_word + 1), size_ - 1);

            for (uint64_t j = i; j < falling_in_idx; ++j) {
                assert(j + 1 < size_);
                set(j, at(j + 1));
            }

            if (falling_in_idx == size_ - 1) {
                set(size_ - 1, 0);
            }
            current_word++;
        }

        // now for the remaining integers we can work blockwise
        for (uint64_t j = current_word; fast_mul(j) < size_; ++j) {
            words[j] >>= 1;
            const auto fval = fast_mul(j + 1);
            falling_in_idx = fval < size_ ? at(fval) : 0;
            set(fast_mul(j) + int_per_word_ - 1, falling_in_idx);
        }
    }

    uint64_t sum(buffered_packed_bit_vector& vec) const {
        uint64_t res = 0;
        for (uint64_t i = 0; i < vec.size(); ++i) {
            res += vec.at(i);
        }
        return res;
    }

    static constexpr uint8_t width_ = 1;
    static constexpr uint8_t int_per_word_ = 64;
    static constexpr uint64_t MASK = 1;
    static constexpr uint8_t extra_ = 2;
    static constexpr uint32_t VALUE_MASK = 1;
    static constexpr uint32_t TYPE_MASK = 8;
    static constexpr uint32_t INDEX_MASK = ~((uint32_t(1) << 8) - 1);

    std::vector<uint64_t> words{};
    uint64_t psum_ = 0;
    uint64_t size_ = 0;

    uint32_t buffer[buffer_size];
    uint8_t buffer_count;
};

}  // namespace dyn