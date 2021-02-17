#!/bin/bash

echo sources

/usr/bin/time ./cw-bwt ../corpus/sources ../corpus/sources_DYNAMIC > /dev/null

echo dna

/usr/bin/time ./cw-bwt ../corpus/dna ../corpus/dna_DYNAMIC > /dev/null

echo xml

/usr/bin/time ./cw-bwt ../corpus/dblp.xml ../corpus/dblp.xml_DYNAMIC > /dev/null

echo E.coli

/usr/bin/time ./cw-bwt ../corpus/E.coli ../corpus/E.coli_DYNAMIC > /dev/null

echo english

/usr/bin/time ./cw-bwt ../corpus/english ../corpus/english_DYNAMIC > /dev/null

echo proteins

/usr/bin/time ./cw-bwt ../corpus/proteins ../corpus/proteins_DYNAMIC > /dev/null
