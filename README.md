# Polybench + pyalp

This is a reorganized version of [Polybench/C benchmark suite][PB] [version 4.2][4.2] 
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

### How to run benchmarks

**Basic usage**

This command executes using the default options listed below.
       
```text
run.sh 
```

<small>Note: use bash shell if on MacOS: `/bin/sh run.sh`</small>

**Available arguments**


| FLAG | DESCRIPTION: options                                            | DEFAULT    |
|:----:|:----------------------------------------------------------------|:-----------|
|  -c  | system compiler to use                                          | `gcc`      |
|  -d  | directory:  `parallel`, `original`                              | `original` | 
|  -o  | optimization level: `O0`, `O1`, `O2`, `O3`, ...                 | `O0`       |
|  -v  | max. variance when timing results (%) : > `0.0`                 | `5.0`      |
|  -s  | dataset size : `MINI`, `SMALL`, `MEDIUM`, `LARGE`, `EXTRALARGE` | `STANDARD` |
|  -a  | all programs should be timed                                    | (not set)  |

| Duration                     |           |
|:-----------------------------|:----------|
| Parallelizable programs only | 3 min     |
| All programs (`-a` flag)     | 30-40 min |


### Results

The results can be found in `results/` directory. Two files will be generated for each run:

1. `[dir]_[timestamp]_model.txt` - machine + processor snapshot, meta data

2. `[dir]_[timestamp].txt` - actual results of timing

Data labels, in order:

- `program`: name of evaluated program
- `variance (%)`: variance of recorded execution times
- `time (s)`: average runtime (clock time), in seconds

Labels are always the same and not included in timing results file.

Timing options are same as default:

- perform 5 executions/program
- take average of 3 runs (exclude min, max time)
- dataset size is `STANDARD_DATASET`

* * *

### Original source

* [Polybench/C @ Ohio State][PB]
* [Ohio State University Software Distribution License](./LICENSE.txt)
* [Descriptions of programs](./polybench.pdf)
* [Original readme](./README)
* [Authors](./AUTHORS) and [Thanks](./THANKS)
* [Download Polybench/C v4.2 @ SourceForge][4.2]

[PB]: http://web.cse.ohio-state.edu/~pouchet.2/software/polybench/ 
[4.2]: https://sourceforge.net/projects/polybench/files/
