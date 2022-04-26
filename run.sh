#!/usr/bin/env bash

# Batch timing script.
#
# This script compiles all examples in prog/ then runs the built-in
# timing benchmark script. Output is stored in a file under `result/`.
#
# basic usage:
# /bin/sh run.sh
#
# specify compiler:
# /bin/sh run.sh -c gcc-10
#
# specify optimization level:
# /bin/sh run.sh -o O1


# parse command line args
while getopts c:o: flag
do
    case "${flag}" in
        c) compiler=${OPTARG};;
        o) opt_level=${OPTARG};;
    esac
done

CC="${compiler:-gcc}"               # compiler, default: gcc
OPT="${opt_level:-O0}"              # optimization level, default: O0
DT=$(date '+%Y%m%d%H%M');           # current timestamp for unique filenames
OUTFILE=./result/"$DT".txt          # where to save timing results
MODEL=./result/"$DT"_model.txt      # where to save machine details
START=$(date '+%H:%M:%S');          # start time

# capture runtime details
echo "# RUNTIME" >>  "$MODEL"
echo "compiler: "$CC"\nopt level: "$OPT"" >>  "$MODEL"
echo "\n# MACHINE" >>  "$MODEL"
echo "OS: "$OSTYPE"" >> "$MODEL"

# capture details of running machine
case "$OSTYPE" in
  linux*)   cat /proc/cpuinfo >> "$MODEL" ;;
  darwin*)  sysctl -a | grep machdep.cpu >> "$MODEL" ;;
  solaris*) echo "no details on SOLARIS"  >> "$MODEL" ;;
  bsd*)     echo "no details on BSD" >> "$MODEL" ;;
  msys*)    echo "no details on WINDOWS" >> "$MODEL" ;;
  cygwin*)  echo "no details on ALSO WINDOWS" >> "$MODEL"  ;;
  *)        echo "no details on unknown: $OSTYPE" >> "$MODEL"  ;;
esac

# clear compiled files if exist
for file in ./compiled/*
do
    rm -rf "$file"
done

# compile and time each example
for file in ./prog/*.c
do
    filename=$(basename -- "$file")
    extension="${filename##*.}"
    filename="${filename%.*}"
    out=./compiled/"$filename"_time

    "$CC" -"$OPT" -I utilities -I prog utilities/polybench.c "$file" -DPOLYBENCH_TIME -o "$out"
    /bin/sh ./utilities/time_benchmark.sh "$out" >> "$OUTFILE"
    echo "done with $file"
done

echo "\n# TIME" >>  "$MODEL"
echo "start: "$START"\nend:   "$(date '+%H:%M:%S')"" >>  "$MODEL"
