#!/usr/bin/env bash

# ROSE compiler helper to transform and parallelize benchmarks.
#
# Usage:
# ./run.sh -r ROSE_path

# parse command line args
while getopts r flag
do
    case "${flag}" in
        r) rose_build=${OPTARG};;
        *) ;;
    esac
done

# Unpack the args and set defaults
ROSE="${rose_build:-/home/rose}/build"      # path to rose compiler (from source), default: /home/rose

# setup other variables
SRC="original"                              # path to benchmark programs, default: ./original
OUT="alt"                                   # path to output directory, default: ./alt
BEG="#pragma scop"                          # kernel start delimiter
END="#pragma endscop"                       # kernel end delimiter

ROSE_ARGS=" --edg:no_warnings -I headers -I utilities "    # common ROSE args

# silenced the output of pushd popd

pushd () {
    command pushd "$@" > /dev/null
}

popd () {
    command popd "$@" > /dev/null
}

# ensure output directory exists
[ -d "$OUT" ] || mkdir "$OUT"

# clear all generated files in the output directory if exist
for file in ./"$OUT"/*
do
    rm -rf "$file"
done

pushd "$OUT"  # switch to output directory, need to run rose inside it

echo "Using ROSE at: $ROSE"

# transform each benchmark in input directory
# assumes $SRC and $OUT are siblings
for file in ../"$SRC"/*.c
do
    # extract benchmark name
    f=$(basename -- "$file")
    p="${f%.*}"
    pf="$p".c

    # read all lines
    IFS=$'\n' read -d '' -r -a lines < $file

    # find templating code
    bi=-1; ei=-1
    for i in "${!lines[@]}"; do
       if [[ "${lines[$i]}" = "$BEG" ]]; then
          bi=$((i+1))
       fi
       if [[ "${lines[$i]}" = "$END" ]]; then
          ei=$i
          break;
       fi
    done
    tmpl_beg=("${lines[@]:0:$bi}")
    tmpl_end=("${lines[@]:$ei}")

    # loop transform
    if( "$ROSE"/tutorial/loopProcessor "$ROSE_ARGS" -w -c -fs0 -cp 0 "$file" ); then
       mv rose_"$pf" "$pf"   # rose will add prefix rose_, remove it
    else
       echo "⚠ cannot transform $p"
       cp "$file" "$pf"   # if transform fails we copy original as-is
    fi

    # AutoPar
    if( "$ROSE"/projects/autoParallelization/autoPar "$ROSE_ARGS" "$p".c ); then
       mv rose_"$p".c "$p".c   # rose will add prefix rose_, remove it
    else
       echo "⚠ cannot parallelize $p"
    fi

    # read all lines
    IFS=$'\n' read -d '' -r -a kernel_lines < "$pf"

    # find kernel
    kb=-1; ke=-1
    for i in "${!kernel_lines[@]}"; do
       if [[ "${kernel_lines[$i]}" = "$BEG" ]]; then
          kb=$((i+1))
       fi
       if [[ "${kernel_lines[$i]}" = "$END" ]]; then
          ke=$((i-kb))
          break;
       fi
    done
    kernel=("${kernel_lines[@]:$kb:$ke}")

    # now piece it together and write to file
    printf "%s\n" "${tmpl_beg[@]}" > "$pf"
    printf "%s\n" "${kernel[@]}" >> "$pf"
    printf "%s\n" "${tmpl_end[@]}" >> "$pf"

done

popd   # exit output directory

