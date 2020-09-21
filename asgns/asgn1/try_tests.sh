#!/bin/bash
make clean && make 
TEST_OUT_FOLDER="test_out_files"
OUT_FOLDER="out_files"
IN_FOLDER="in_files"

rm -f "$TEST_OUT_FOLDER/*"
for f in $(ls $IN_FOLDER)
do
    echo "For ------$f------------------------------"
    ./trace "$IN_FOLDER/$f" > "$TEST_OUT_FOLDER/$f.out"
    diff -B -u -p -w "$TEST_OUT_FOLDER/$f.out" "$OUT_FOLDER/$f.out"
    echo "--------------------------------------------"
done
