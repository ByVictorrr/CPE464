#!/bin/bash
make clean && make 
TEST_OUT_FOLDER="test_out_files"
OUT_FOLDER="out_files"
IN_FOLDER="in_files"

RESULTS="valgrind"
rm -f "$TEST_OUT_FOLDER/*"
for f in $(ls $IN_FOLDER)
do
    ./trace "$IN_FOLDER/$f" > "$TEST_OUT_FOLDER/$f.out"
    echo "-----------------$f------------------------" 
    valgrind --leak-check=full --log-file="$RESULTS/$f.val" ./trace "$IN_FOLDER/$f" &> /dev/null
    echo "+++++++++++++++++diff++++++++++++++++++++++"
    diff -B -u -p -w "$TEST_OUT_FOLDER/$f.out" "$OUT_FOLDER/$f.out"
    echo "++++++++++++++++++++++++++++++++++++++++++"
    echo "--------------------------------------------"
done
