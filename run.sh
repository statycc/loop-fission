#!/usr/bin/env bash

# Batch timing script.
#
# This script compiles all examples in prog/
# then runs the built-in timing benchmark script.
# Output is stored in a file under `result/`
#
# basic usage:
# /bin/sh run.sh
#
# specify compiler:
# /bin/sh run.sh -c gcc-10
#
# specify opt level:
# /bin/sh run.sh -o O1
#

while getopts c:o: flag
do
    case "${flag}" in
        c) compiler=${OPTARG};;
        o) opt_level=${OPTARG};;
    esac
done

CC="${compiler:-gcc}"           # compiler, default: gcc
OPT="${opt_level:-O0}"          # optimization level, default: O0
DT=$(date '+%Y%m%d_%H%M%S');

for file in ./compiled/*
do
    rm -rf "$file"
done

echo "CC=$CC OPT_LEVEL=$OPT\n" >> ./result/run_"$DT"

for file in ./prog/*.c
do
    filename=$(basename -- "$file")
    extension="${filename##*.}"
    filename="${filename%.*}"
    out=./compiled/"$filename"_time

    "$CC" -"$OPT" -I utilities -I prog utilities/polybench.c "$file" -DPOLYBENCH_TIME -o "$out"
    /bin/sh ./utilities/time_benchmark.sh "$out" >> ./result/run_"$DT"
    echo "done with $file"
done

