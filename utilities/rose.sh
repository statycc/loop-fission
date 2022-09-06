#!/usr/bin/env bash

# ROSE compiler helper to transform and parallelize benchmarks.
#
# Usage:
# ./run.sh -r ROSE_path -p prog_name
#
# all args are optional

# parse command line args
while getopts r:p: flag
do
    case "${flag}" in
        r) rose_build=${OPTARG};;
        p) prog=${OPTARG};;
        *) ;;
    esac
done

# Unpack the args and set defaults
ROSE="${rose_build:-/home/rose}/build"      # path to rose compiler (from source), default: /home/rose
PROGRAM=${prog}                             # specific program

# setup other variables
SRC="original"                              # path to benchmark programs
OUT="alt"                                   # path to output directory
BEG="#pragma scop"                          # kernel start delimiter
END="#pragma endscop"                       # kernel end delimiter

ROSE_ARGS=" --edg:no_warnings -I../headers -I../utilities "    # common ROSE args

# silenced the output of pushd popd

pushd () {
    command pushd "$@" > /dev/null
}

popd () {
    command popd "$@" > /dev/null
}

# ensure output directory exists
[ -d "$OUT" ] || mkdir "$OUT"

pushd "$OUT"  # switch to output directory, need to run rose inside it

echo "Using ROSE at: $ROSE"

# transform each benchmark in input directory
# assumes $SRC and $OUT are siblings
for file in ../"$SRC"/*.c
do

    ok="\033[1;32m✓\033[0m"
    neg="\033[0;91m✕\033[0m"
    trans=$neg
    paral=$neg
    resto=$neg

    # extract benchmark name
    f=$(basename -- "$file")
    p="${f%.*}"
    pf="$p".c

    # only specific program
    if [ -n "$PROGRAM" ]; then
        [[ "$PROGRAM" != "$p" ]] && continue
    fi

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
    {
       cmd="${ROSE}/tutorial/loopProcessor ${ROSE_ARGS} -w -c -fs0 -cp 0 ${file}"
       command $cmd > /dev/null 2>&1
       if test -f "rose_${pf}"; then
         mv rose_${pf} ${pf}
         trans=$ok
       else
         cp  "$file" "$pf"
       fi
    } || {
       cp "$file" "$pf"   # if transform fails we copy original as-is
    }

    # AutoPar
    cmd="${ROSE}/projects/autoParallelization/autoPar ${ROSE_ARGS} -c ${pf}"
    command $cmd > /dev/null 2>&1
    if test -f "rose_${pf}"; then
       mv rose_${pf} ${pf}
       paral=$ok
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

    # substitute kernel inside template if found
    if (( $kb > 0 )); then
      kernel=("${kernel_lines[@]:$kb:$ke}")
      printf "%s\n" "${tmpl_beg[@]}" > "$pf"
      printf "%s\n" "${kernel[@]}" >> "$pf"
      printf "%s\n" "${tmpl_end[@]}" >> "$pf"
      resto=$ok
    fi

    echo -e "$(printf '%-12s' $p) transformed: $trans   parallel: $paral   restored: $resto"

done

popd   # exit output directory

for file in ./"$OUT"/*.o
do
    rm -rf "$file"
done
