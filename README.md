# icc-fission Benchmarks

This repository is for benchmarking the ICC-fission algorithm presented in
_"A Novel Loop Fission Technique Inspired by Implicit Computational Complexity"_.

It is a based on [Polybench/C benchmark suite][PB] [version 4.2][4.2]
benchmark suite, containing programs to which the loop fission technique can be
applied (6 examples). After applying the transformation, the program is then
parallelized using OpenMP directives to evaluate the resulting efficiency.

Annotating the transformed program with parallelization directives is outside
the scope of the algorithm, therefore we have used here two different
approaches:

1. manually inserted annotations
2. automated annotations using [`autopar-clava`](https://github.com/specs-feup/clava).

## Organization of programs

| Directory          | Loop fission | Parallel | Parallelization<br/>technique | Description                              |
|:-------------------|:------------:|:--------:|:------------------------------|:-----------------------------------------|
| `original`         |      ✖️      |    ✖️    | N/A                           | unmodified programs from PB/C suite      |
| `original_autopar` |      ✖️      |    ✔️    | `autopar-clava`               | parallel versions of original programs   |
| `parallel_autopar` |      ✔️      |    ✔️    | `autopar-clava`               | with loop fission transformation applied | 
| `parallel_manual ` |      ✔️      |    ✔️    | by hand                       | with loop fission transformation applied |  

**Other directories and files**

* `_annotated/` directories are preprocessed versions, for use
  by `autopar-clava`. These are intermediate versions of programs, not relevant
  for purposes of benchmarking.

* `headers/` directory contains the original header files from PB/C suite, moved
  to a separate directory for purposes of sharing.

* `results/` contains captured benchmark results

* `utilities/` are from PB/C suite and contains e.g. the benchmarking script.

* `run.sh` is a wrapper for the timing script in utilities. It adds some command
  line arguments and options to ease benchmarking full directories of programs.

### How to run benchmarks

**Basic usage**

This command executes using the default options listed below.
       
```text
./run.sh 
```

If necessary change permissions: `chmod u+r+x ./run.sh`

**Available arguments**


| FLAG | DESCRIPTION: options                                                    | DEFAULT     |
|:----:|:------------------------------------------------------------------------|:------------|
|  -c  | system compiler to use                                                  | `gcc`       |
|  -d  | directory:  `parallel`, `original`                                      | `original`  | 
|  -o  | optimization level: `O0`, `O1`, `O2`, `O3`, ...                         | `O0`        |
|  -v  | max. variance when timing results (%) : > `0.0`                         | `5.0`       |
|  -s  | data size: `MINI`, `SMALL`, `MEDIUM`, `LARGE`, `EXTRALARGE`, `STANDARD` | `STANDARD`  |

**Duration**

EXTRALARGE data without optimization: 10-15 min, less when using smaller sizes, compiler optimization.

### Results

The results can be found in `results/` directory. Two files will be generated for each run:

1. `[args]_[timestamp]_model.txt` - machine + processor snapshot, meta data

2. `[args]_[timestamp].txt` - actual results of timing

Data labels, in order:

- `program`: name of evaluated program
- `variance (%)`: variance of recorded execution times
- `time (s)`: average runtime (clock time), in seconds

Labels are always the same and not included in timing results file.

Timing options are same as default:

- perform 5 executions/program
- take average of 3 runs (exclude min, max time)

* * *

### Original source

* [Information about Polybench/C @ Ohio State][PB]
* [Ohio State University Software Distribution License](./LICENSE.txt)
* [Download Polybench/C v4.2 @ SourceForge][4.2]

[PB]: http://web.cse.ohio-state.edu/~pouchet.2/software/polybench/ 
[4.2]: https://sourceforge.net/projects/polybench/files/
