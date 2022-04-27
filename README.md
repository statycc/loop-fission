# Polybench + pyalp

This is a reorganized version of [Polybench/C benchmark suite][PB], [version 4.2][4.2], 
with minimal changes and extensions, for purpose of timing pyalp transformations 
on these programs.

**Changes to original suite:**

1. The original benchmark program files are under `original/`
    - these are in a flat directory, unlike original source.
    
2. Headers moved to `headers` to enable sharing between the program directories
    - references to headers have been changed from e.g. `"2mm.h" -> <2mm.h>`

3. `time_benchmark.sh` changed to output results in tabular format

**Extensions:**

1. Parallel versions of programs in `parallel/` directory

    - These are manually transformed versions of the benchmark programs 
    - Filename schema:
        - Filename ends with `_og.c` =>  non-parallelizable/same as original version
        - Otherwise: file has been transformed and is parallelizable
        
2. `run.sh` script has been added 

    - this is a wrapper for the built-in timing script
    - enables timing entire directory at once
    - adds some useful argument flags

### How to time


**Basic usage**

```text
run.sh 
```

Note: use bash shell if on MacOS: `/bin/sh run.sh`

This command executes using the default options listed below.
       
**Available arguments**

```
-c  compiler to use                             [default: gcc]
-d  directory to benchmark (parallel|original)  [default: original]
-o  optimization level e.g O3                   [default: O0] 
-s  skip programs that are not parallelizable   [default: false]
```

### Results

The results can be found in `results/` directory. There are two files per run: 

1. `[dir]_[timestamp]_model.txt`: machine + processor snapshot, other meta data

2. `[dir]_[timestamp].txt`: actual results of timing

Data labels are: `program, variance (%), time (s)`.
Labels are always the same and not include in timing file.

Timing options are same as default

- 5 runs, take avg 3 runs (not min, max),
- dataset size `STANDARD_DATASET`

Duration

- all examples: ~ 30-40 min
- parallelized only (use `-s` flag): ~ 3 min

* * *

### Original source

* [Polybench/C @ Ohio State][PB]
* [Ohio State University Software Distribution License](./LICENSE.txt)
* [Descriptions of programs](./polybench.pdf)
* [Original readme](./README)
* [Authors](./AUTHORS) and [Thanks](./THANKS)
* [Download v4.2 @ SourceForge][4.2]

[PB]: http://web.cse.ohio-state.edu/~pouchet.2/software/polybench/ 
[4.2]: https://sourceforge.net/projects/polybench/files/