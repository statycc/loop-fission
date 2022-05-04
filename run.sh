#!/usr/bin/env bash

# Batch timing script.
#
# This script compiles all examples in source directory then runs the built-in
# timing benchmark script. Output is stored in a timestamped file.
#
# basic usage:
# ./run.sh


# parse command line args
while getopts c:o:d:v:s:p: flag
do
    case "${flag}" in
        c) compiler=${OPTARG};;
        o) opt_level=${OPTARG};;
        d) directory=${OPTARG};;
        v) max_var=${OPTARG};;
        s) size=${OPTARG};;
        p) prog=${OPTARG};;
    esac
done

VALID_SIZES=("MINI","SMALL","MEDIUM","LARGE","EXTRALARGE","STANDARD")

# Unpack the args and set defaults
CC="${compiler:-gcc}"                       # compiler, default: gcc
OPT="${opt_level:-O0}"                      # optimization level, default: O0
SRC="${directory:-original}"                # source directory, default: original
MAX_VARIANCE="${max_var:-5.0}"              # max allowed variance b/w timing results
DS_SIZE=${size:-STANDARD}                   # dataset size: MINI, SMALL, MEDIUM, LARGE, EXTRALARGE
PROGRAM=${prog}                             # benchmark specific program

# configure other runtime options
MAX_RETRIES=100                             # stop repeating after N retries
START=$(date '+%H:%M:%S');                  # start time
DT=$(date '+%m%d%H%M%S');                   # current timestamp

# output directories
CDIR="compiled_"$SRC""                      # for holding compiled programs
RES_DIR="results"                           # where to save results

# output filenames
PATTERN="$SRC"_"$OPT"_"$DS_SIZE"
OUTFILE=./"$RES_DIR"/"$PATTERN".txt       # where to save timing results
MODEL=./"$RES_DIR"/"$PATTERN"_model.txt   # where to save machine details

# check that data size is valid
inarray=$(echo ${VALID_SIZES[@]} | grep -o "$DS_SIZE" | wc -w  | xargs)

if [[ $inarray == "0" ]]; then
    echo "[ERROR]: "$DS_SIZE" is not a valid choice for data size";
    printf "Use one of: "
    for elem in ${VALID_SIZES[*]}; do
        printf '%s ' "$elem"
    done
    echo ""
    exit 1;
fi

# ensure dirs exist
[ -d "$CDIR" ] || mkdir "$CDIR"
[ -d "$RES_DIR" ] || mkdir "$RES_DIR"

# capture runtime details
echo "# RUNTIME" >>  "$MODEL"
echo "compiler: "$CC"" >>  "$MODEL"
echo "opt level: "$OPT"" >>  "$MODEL"
echo "source: "$SRC"" >>  "$MODEL"
echo "data size: "$DS_SIZE"" >>  "$MODEL"
echo "# MACHINE" >>  "$MODEL"
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
rm -rf "$OUTFILE"

# compile and time each example
for file in ./"$SRC"/*.c
do

    i=0 # counter for timing retries

    # get filename without extension
    filename=$(basename -- "$file")
    extension="${filename##*.}"
    filename="${filename%.*}"
    out=./"$CDIR"/"$filename"_time

    # benchmark only specific program
    if [ -n "$PROGRAM" ]; then
        [[ "$PROGRAM" != "$filename" ]] && continue
    fi

    # compile options
    "$CC" -"$OPT" -lm -fopenmp -I utilities -I headers utilities/polybench.c "$file" -DPOLYBENCH_TIME -D"$DS_SIZE"_DATASET -o "$out"

    while : ; do

        # run benchmark
        result=$(/bin/sh ./utilities/time_benchmark.sh "$out")

        result_arr=($result)            # split by whitespace
        variance=${result_arr[1]}       # get recorded variance
        i=$((i+1))                      # iteration counter

        # if variance is withing allowed range, or max retries exhausted
        if(( $(bc <<< "$variance < $MAX_VARIANCE") )) || [ $i -gt $MAX_RETRIES ]; then
            echo "$result" >> "$OUTFILE"
            if [ $i -gt 0 ]; then
                printf '\r'
                        cols="$(tput cols)"
                        for i in $(seq "$cols"); do
                                printf ' '
                        done
                printf '\r'
            fi

            echo "✓ done with ("$DS_SIZE", -"$OPT"): ${filename}"
            printf '\r'
            break
        else
            echo -ne "⚠ $filename - repeating $i of $MAX_RETRIES - variance too high: ${variance} %\033[0K\r"
        fi

    done
done

echo "# TIME" >>  "$MODEL"
echo "start: "$START"" >>  "$MODEL"
echo "end:   "$(date '+%H:%M:%S')"" >>  "$MODEL"
