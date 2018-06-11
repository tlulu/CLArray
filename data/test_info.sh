#!/bin/bash

fname=$1
words_counts="$(awk '{print NF}' ${fname})"
num_rows=$(wc -l < ${fname} | tr -d '[:space:]')
min_cols=$(echo "${words_counts}" | sort -n | head -n 1)
max_cols=$(echo "${words_counts}" | sort -n -r | head -n 1)

echo "Number of rows: ${num_rows}"
echo "Number of columns: ${min_cols} - ${max_cols}"
