#!/bin/bash

echo Buffered:

/usr/bin/time ./add_bench -b 10000000000 100 > big_scale_buf.tsv

echo Unbuffered:

/usr/bin/time ./add_bench -u 10000000000 100 > big_scale_unb.tsv

echo DYNAMIC:

/usr/bin/time ./add_bench -s 10000000000 100 > big_scale_DYN.tsv