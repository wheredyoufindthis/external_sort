#!/usr/bin/env bash

if [ ! -f input_file ]; then
    ./file_generator.py --max-length 1000 --max-size 50000000 --output input_file --max-buffer-size 100000
fi

./build/external_sort/bin/binary input_file sorted_file && \
./check_sort.py --file-path sorted_file && \
sort input_file > expected_file && diff expected_file sorted_file && \
echo 'test is passed since there is no diff lines in output'
