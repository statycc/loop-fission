# Loop Fission Benchmarks

This repository is for benchmarking the loop fission algorithm presented in _"TBD"_.

This is a curated benchmark suite. The included benchmarks have been selected specifically 
for offering opportunity to perform loop fission transformation. For each benchmark, we measure 
difference in clock time, after loop fission and parallelization using [OpenMP](https://www.openmp.org/).

We compare the original baseline programs to those generated by our loop fission technique.
We also compare our technique to an alternative loop fission technique used in the
[ROSE compiler](http://rosecompiler.org/).

## Files and organization

### Benchmark directories

| Directory  | Description                                                     |
|:-----------|:----------------------------------------------------------------|
| `original` | baseline, unmodified programs                                   | 
| `fission`  | transformed and parallelized programs, using our method         |
| `alt`      | transformed and parallelized programs, using alternative method |

<details>

  <summary>A more detailed explanation of benchmark transformations</summary>

   <br/>In this repository, transformations of benchmarks have already been applied in the appropriate directories,
   <code>fission</code> and <code>alt</code>, and we measure differences between those transformations and 
   <code>original</code>. This section explains briefly how the transformations are generated in the first place.
   
   <a href='./fission'><strong>Fission</strong></a> benchmarks have been transformed manually following our loop 
   fission algorithm. These transformations are not reproducible mechanically. The programs have been parallelized 
   by hand. We use manual approach here because automatic parallelization of <code>while</code> loops is not supported 
   by any tool we are aware of. Programs with <code>for</code> loops could be parallelized automatically, and as 
   shown in the next case.
   
   <a href='./alt'><strong>Alt</strong></a> benchmarks are the results of applying automatic transformation using
   <a href="http://rosecompiler.org/">ROSE compiler</a>. They are also automatically parallelized using the same tool.
   Benchmark <code>remap</code> fails during transformation, and we measure no difference between the original version.
   Because the tool transforms all code, including the timing code of the benchmark template, as last step we
   restore the original benchmark template. Detailed steps for re-generating the alt-benchmarks are
   <a href="/utilities/readme.md">here</a>.

</details>


### Benchmark descriptions

All programs are written in C language. We include programs with both `for` and `while` loops. 

| Benchmark  | Description                                    | for | while | Origin            |
|:-----------|:-----------------------------------------------|:---:|:-----:|:------------------|
| `3mm`      | 3D matrix multiplication                       |  ✔  |       | [PolyBench/C][PB] | 
| `bicg`     | BiCG sub kernel of BiCGStab linear solver      |     |   ✔   | [PolyBench/C][PB] | 
| `colormap` | TIFF image conversion of photometric palette   |     |   ✔   | [MiBench][MB]     | 
| `conjgrad` | Conjugate gradient routine                     |  ✔  |       | [NAS-CG][NAS]     |
| `cp50`     | Ghostscript/CP50 color print routine           |  ✔  |   ✔   | [MiBench][MB]     | 
| `deriche`  | Edge detection filter                          |     |   ✔   | [PolyBench/C][PB] | 
| `fdtd-2d`  | 2-D finite different time domain kernel        |  ✔  |       | [PolyBench/C][PB] |
| `gemm`     | Matrix-multiply C=alpha.A.B+beta.C             |  ✔  |       | [PolyBench/C][PB] |
| `gesummv`  | Scalar, vector and matrix multiplication       |     |   ✔   | [PolyBench/C][PB] | 
| `mvt`      | Matrix vector product and transpose            |     |   ✔   | [PolyBench/C][PB] | 
| `remap`    | 4D matrix memory remapping                     |  ✔  |       | [NAS-UA][NAS]     |
| `tblshift` | TIFF PixarLog compression main table bit shift |  ✔  |   ✔   | [MiBench][MB]     |

[PB]: http://web.cse.ohio-state.edu/~pouchet.2/software/polybench/
[NAS]: https://www.nas.nasa.gov/software/npb.html
[MB]: https://vhosts.eecs.umich.edu/mibench

### Other directories and files

* `headers/` header files for benchmark programs.

* `utilities/` e.g. the timing script, obtained from
   [PolyBench/C][PB] benchmark suite [version 4.2](https://sourceforge.net/projects/polybench/files/).

* `plot.py` is used for generating tables and plots from results.

* `run.sh` is a wrapper for the timing script; it enables benchmarking directories.

The folders `results` and `plots` are discussed below.

**Example provided**

The files in results and plots are provided for convenience, as an example of the results we have obtained, 
and can safely be deleted or overwritten. They were obtained on a Linux 4.19.0-20-amd64 #1 SMP Debian 
4.19.235-1 (2022-03-17) x86_64 GNU/Linux machine, with 4 Intel(R) Core(TM) i5-6300U CPU @ 2.40GHz processors.

**Add plot/table here**

## Reproducing results

**System requirements:** Linux/OSX host, C compiler with OpenMP support, 
Python 3+ for plotting. The system should have multiple cores for parallelism,
but 4 cores is sufficient for this experiment.

Benchmarking proceeds in two phases: first capture the timing results, then generate
plots and tables from those results. Details of these steps follow next.

### Running the benchmarks

**Small evaluation** — compiles and times partial benchmarks — :timer_clock: 10-15 min.

```text
make small
```

**Run all benchmarks** — compiles and times the execution of all benchmarks — :timer_clock: 120 min.

```text
make all
```

System should include a C compiler that supports OpenMP pragmas (assumes GCC).
You may specify an alternative compiler using `make all CC=compiler_here`.

**Custom execution** — call the `run.sh` script directly: 
       
```text
./run.sh 
```

**Available arguments for timing**

| ARGUMENT | DESCRIPTION: options                                                    | DEFAULT    |
|:---------|:------------------------------------------------------------------------|:-----------|
| `-c`     | system compiler to use                                                  | `gcc`      |
| `-d`     | which directory:  `original`, `fission`, `alt`                          | `original` | 
| `-o`     | optimization level: `O0`, `O1`, `O2`, `O3`, ...                         | `O0`       |
| `-v`     | max. variance (%) when timing results: > `0.0`                          | `5.0`      |
| `-s`     | data size: `MINI`, `SMALL`, `MEDIUM`, `LARGE`, `EXTRALARGE`, `STANDARD` | `STANDARD` |
| `-p`     | benchmark specific program: `3mm`, `bicg`, `deriche` ...                | _not set_  |

If necessary, change permissions: `chmod u+r+x ./run.sh`.

### Locating and interpreting results

The results can be found in `results/` directory, categorized by source directory name, 
optimization level and data size. Two files will be generated for each category, for each run:

1. `[args]_model.txt` - machine + processor snapshot, meta data

2. `[args].txt` - actual results of timing.

Data labels, in order:

- `program`: name of evaluated program
- `variance (%)`: variance of recorded execution times
- `time (s)`: average runtime (clock time), in seconds

Labels are always the same. They are not included in timing results file.

Timing options are same as default:

- perform 5 executions/program
- take average of 3 runs (min and max time are excluded)
- max variance controls the (%) allowed between the 3 remaining runs

### Generating plots and tables

After capturing results, use the plotting script to generate tables or graphs. This step requires Python version 3.+

1. Install dependencies

    ```text
    python -m pip install -q -r requirements.txt
    ```

2. Generate tables or plots

    ```text
    make plots
    ```

To customize plot options, call the `plot.py` directly with selected arguments.

**Available arguments for plotting**

```text
python plot.py --help
```

| ARGUMENT        | DESCRIPTION : options                                                    | DEFAULT    |
|:----------------|:-------------------------------------------------------------------------|------------|
| `--data`        | data choice: `time`, `speedup`                                           | `time`     |
| `--out`         | path to output directory                                                 | `plots`    |
| `--fmt`         | output format: `tex`, `md`, `plot`                                       | `md`       |
| `--digits`      | number of digits for tabular values: `0`...`15`                          | `6`        |
| `--ss`          | source directory for calculating speedup                                 | `original` |
| `--st`          | target directory for calculating speedup (all when not set)              | _not set_  |
| `--millis`      | display table of times in milliseconds  (otherwise in seconds)           | _not set_  |
| `--show`        | show generated plot or table                                             | _not set_  |
| `--dir_filter`  | include directories (comma-separated list): `original`, `fission`, `alt` | _not set_  |
| `--prog_filter` | include programs (comma-separated list): `3mm`, `bicg`, `deriche` ...    | _not set_  |
| `--help`        | show help message and exit                                               | _not set_  |


