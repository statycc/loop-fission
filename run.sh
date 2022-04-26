#!/usr/bin/env bash

# Batch timing script.
#
# This script compiles all examples in prog/ then runs the built-in
# timing benchmark script. Output is stored in a file under `result/`.
#
# basic usage:
# /bin/sh run.sh
#
# specify source directory:
# /bin/sh run.sh -d parallel
# /bin/sh run.sh -d original
#
# specify compiler:
# /bin/sh run.sh -c gcc-10
#
# specify optimization level:
# /bin/sh run.sh -o O1


# parse command line args
while getopts c:o:d: flag
do
    case "${flag}" in
        c) compiler=${OPTARG};;
        o) opt_level=${OPTARG};;
        d) directory=${OPTARG};;
    esac
done

RES_DIR="result"
CC="${compiler:-gcc}"                       # compiler, default: gcc
OPT="${opt_level:-O0}"                      # optimization level, default: O0
SRC="${directory:-original}"                # source directory, default: original
DT=$(date '+%y%m%d%H%M');                   # current timestamp for unique filenames
OUTFILE=./"$RES_DIR"/"$SRC"_"$DT".txt       # where to save timing results
MODEL=./"$RES_DIR"/"$SRC"_"$DT"_model.txt   # where to save machine details
START=$(date '+%H:%M:%S');                  # start time
CDIR="compiled_"$SRC""

# ensure dirs exist
[ -d "$CDIR" ] || mkdir "$CDIR"
[ -d "$RES_DIR" ] || mkdir "$RES_DIR"

# capture runtime details
echo "# RUNTIME" >>  "$MODEL"
echo "compiler: "$CC"\nopt level: "$OPT"\nsource: "$SRC"" >>  "$MODEL"
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
for file in ./"$CDIR"/*
do
    rm -rf "$file"
done

# compile and time each example
for file in ./"$SRC"/*.c
do

    # get filename without extension
    filename=$(basename -- "$file")
    extension="${filename##*.}"
    filename="${filename%.*}"
    out=./"$CDIR"/"$filename"_time

    [[ "$filename" == _* ]] && continue  # ignore non-transformed

    # compile options add -lm -fopenmp
    "$CC" -"$OPT" -I utilities -I prog utilities/polybench.c "$file" -DPOLYBENCH_TIME -o "$out"

    # run benchmark
    /bin/sh ./utilities/time_benchmark.sh "$out" >> "$OUTFILE"

    echo "done with $file"
done

echo "\n# TIME" >>  "$MODEL"
echo "start: "$START"\nend:   "$(date '+%H:%M:%S')"" >>  "$MODEL"
