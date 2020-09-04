
# Attempted improvements to: 

## DYNAMIC: a succinct and compressed fully-dynamic data structures library

===============

### Based on

a fork of the repository https://github.com/nicolaprezza/dynamic

@inproceedings{prezza2017framework,  
  title={A Framework of Dynamic Data Structures for String Processing},  
  author={Prezza, Nicola},  
  booktitle={International Symposium on Experimental Algorithms},  
  year={2017},  
  organization={Leibniz International Proceedings in Informatics (LIPIcs)}  
}

### Data structures

Contains 2 additional types

```c++
typedef succinct_bitvector<spsi<buffered_packed_bit_vector<8>,8192,16>> b_suc_bv;

typedef succinct_bitvector<spsi<buffered_packed_bit_vector<0>,8192,16>> ub_suc_bv;
```

These are based on the new `buffered_packed_bit_vector`, that is an improved version of the `packed_bit_vector` of the original DYNAMIC.

All operations on these new structures seem to be significantly faster than on the `suc_bv` type.

The gains are due to code optimizations and buffering.

Due to the buffering, some guarantees on space complexity of `b_suc_bv` are loosened. Practically better than 3 bits per bit is guaranteed for large vectors and better than 1.5 bits per bit is expected.
