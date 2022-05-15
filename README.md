# ICC-Fission Benchmarks

This repository is for benchmarking the ICC-fission algorithm presented in _"A Novel Loop Fission Technique Inspired by Implicit Computational Complexity"_.

It is a based on [PolyBench/C benchmark suite][PB] [version 4.2][4.2], containing the subset of programs to which the loop fission technique can be applied (6 examples). After applying the transformation, the program is then parallelized using OpenMP directives to evaluate the resulting efficiency.

Annotating the transformed program with parallelization directives is outside the scope of the algorithm. We have used two different approaches:

1. manually inserted annotations
2. automated annotations using [`autopar-clava`](https://github.com/specs-feup/clava).

The two approaches serve different purposes: manual method enables finding optimal directives, but automatic method shows these tools can be pipelined, to work without human interaction.

## Organization of programs

| Directory          | Loop fission | Parallel | Description                                       |
|:-------------------|:------------:|:--------:|:--------------------------------------------------|
| `original`         |      ➖       |    ➖     | unmodified programs from PB/C suite               | 
| `original_autopar` |      ➖       |    ✅     | original programs, parallelized automatically[^1] |
| `fission_autopar`  |      ✅       |    ✅     | with loop fission, parallelized automatically[^1] | 
| `fission_manual `  |      ✅       |    ✅     | with loop fission, parallelized by hand           |
| `case_study-a`     |      ➖       |    ✅     | Case studies: baseline versions                   | 
| `case_study-b`     |      ✅       |    ✅     | Case studies: alternative versions for comparison |

[^1]: [`autopar-clava`](https://github.com/specs-feup/clava) [edits significantly the original source code](https://github.com/specs-feup/specs-lara/issues/1), outside of the function that needs to be parallelized, strictly speaking. This affects the original timing used by PolyBench/C. As a consequence, those files have been obtained by running [our script script_autopar.sh](script_autopar.sh) to obtain the version with openmp annotation, and then those annotations have been placed in the original file.


**Other directories and files**

* `fission/` non-parallelized versions of programs after applying loop fission.

* `headers/` directory contains the original header files from PB/C suite, moved to a separate directory for purposes of sharing.

* `utilities/` are from PB/C suite and contains e.g. the benchmarking timing script.

* `run.sh` is a wrapper for the timing script in utilities. It adds some command line arguments and options to ease benchmarking full directories of programs at once.

* `script_autopar.sh` is used for inserting automatic parallelization directives. 
  
* `plot.py` is used for generating tables and plots from results.  


The files in `results` and `plots` are provided for convenience, as an example of the results we have obtained, and can safely be deleted or overwritten.
They were obtained on a Linux 4.19.0-20-amd64 #1 SMP Debian 4.19.235-1 (2022-03-17) x86\_64 GNU/Linux  machine, with 4 Intel(R) Core(TM) i5-6300U CPU @ 2.40GHz processors.

They show appreciable gains, first on the "case_study" example (parallelization of `while` loop, outside the scope of most automatic parallelization tool):

![Gain on parallelization of while loops](plots/speedup_case_study-a-case_study-b.png "Speedup obtained by parallelizing the `while` loop in the case\_study program")

## How to run benchmarks

**Basic usage** 

Benchmark PB/C programs in all 4 directories

```text
make everything
```

Benchmark PB/C `original` vs. `fission_manual`

```text
make all
```

For customizable options, call the `run.sh` script directly, with choice of arguments: 
       
```text
./run.sh 
```

**Available arguments**

| FLAG | DESCRIPTION: options                                                    | DEFAULT     |
|:----:|:------------------------------------------------------------------------|:------------|
| `-c` | system compiler to use                                                  | `gcc`       |
| `-d` | directory:  `original`, `original_autopar`, `fission_autopar`,...       | `original`  | 
| `-o` | optimization level: `O0`, `O1`, `O2`, `O3`, ...                         | `O0`        |
| `-v` | max. variance when timing results (%) : > `0.0`                         | `5.0`       |
| `-s` | data size: `MINI`, `SMALL`, `MEDIUM`, `LARGE`, `EXTRALARGE`, `STANDARD` | `STANDARD`  |
| `-p` | benchmark specific program: `3mm`, `bicg`, `deriche` ...                | _not set_   |

If necessary, change permissions: `chmod u+r+x ./run.sh`

**Duration**

For a machine with 4 cores

- Benchmark all 4 program directories:  80-100 min
- original programs only (not parallel):  40-50 min
- parallel directories: 12-20 min/each

### Results

The results can be found in `results/` directory. Two files will be generated for each run:

1. `[args]_model.txt` - machine + processor snapshot, meta data

2. `[args].txt` - actual results of timing

Data labels, in order:

- `program`: name of evaluated program
- `variance (%)`: variance of recorded execution times
- `time (s)`: average runtime (clock time), in seconds

Labels are always the same and not included in timing results file.

Timing options are same as default:

- perform 5 executions/program
- take average of 3 runs (exclude min, max time)

### Plotting

After capturing results, use the plotting script to generate tables or graphs. This step requires Python version 3.+ 

1. Install dependencies
    
    ```text
    python -m pip install -q -r requirements.txt
    ```

2. Generate tables or plots

    ```text
    make plots
    ```

To customize plot options, call the `plot.py` directly with selected arguments


**Available arguments** 

| ARGUMENT                    | DESCRIPTION                                                                                |
|:----------------------------|:------------------------------------------------------------------------------------------------------------------------|
| `-d DATA`, `--data DATA`    | data choice: {`time`, `speedup`}                                                                                        |
| `-o OUT`, `--out OUT`       | output directory [default: `plots`]                                                                                     |
| `-f FMT`, `--fmt FMT`       | output format: {`tex`, `md`, `plot`}                                                                                    |
| `--ss SS`                   | source directory for speedup [default: `original`]                                                                      |
| `--st ST`                   | target directory for speedup [default: _(not set)_]                                                                     |
| `--millis`                  | display table of times in milliseconds, not seconds                                                                     |
| `--digits [0-15]`           | number of digits for tabular values [default: `6`]                                                                      |
| `--show`                    | show generated plot or table                                                                                            |
| `--dir_filter DIR_FILTER`   | Comma separated list of directories to consider<br/>[default: `original,original_autopar,fission_autopar,fission_manual`] |
| `--prog_filter PROG_FILTER` | Comma separated list of programs to consider [default: _(not set)_]                                                     |
| `-h`, `--help`              | show help message and exit                                                                                              |
 
 
To view the various available arguments run: `python plot.py --help`

## Regenerating automatic parallelization directives 

The openmp annotations in the program files in `original_autopar` and `fission_autopar` were obtained by running the source-to-source compiler [AutoPar-Clava](https://dx.doi.org/10.1007/s11227-019-03109-9/) from [clava](https://github.com/specs-feup/clava) on the selected files from the PolyBench benchmarking suite in [original](./original), and on the same files after they have been "split" by our algorithm, as shared in [fission](./fission).

To recreate those files:

1. Install Clava, per their [resources](https://github.com/specs-feup/clava#resources). For anything but windows,

    - Download [their installation script](http://specs.fe.up.pt/tools/clava/clava-update), place it in the folder where you would like to install clava (e.g., `/opt`),
    
    - Execute the installation script as root.

2. Set `clara_path` to the right path if it is not `/opt/clava/Clava/Clava.jar` in [our script script_autopar.sh](script_autopar.sh).

3. Run the automatic annotation script: 

    ```text
    ./script_autopar.sh [source-dir]
    ``` 
    
    For source directory specify either `original` or `fission`, and the result will be in `_original_autopar` and `_fission_autopar`.
    

This script will perform following steps automatically, for each .c file:

- Create the appropriate AutoPar.lara directions (that simply ask to fissionize all the loops in the method function that starts with kernel_),
- Run clava,
- Copy the optimized file to this folder,
- Delete the temporary file,

Note that the files shared in `original_autopar` and `fission_autopar` are obtained by "extracting" the openmp annotation from the resulting files and inserting them in the original files[^1].


* * *

## About 

### PolyBench/C

* [Information about PolyBench/C @ Ohio State][PB]
* [Download PolyBench/C v4.2 @ SourceForge][4.2] ([Ohio State University Software Distribution License](./LICENSE.txt))

### AutoPar-Clava

* [Information about Clava](https://github.com/specs-feup/clava/) ([Apache License](https://github.com/specs-feup/clava/blob/master/LICENSE))
* [The specs-lara repository](https://github.com/specs-feup/specs-lara), where the PolyBench/C examples are shared before and after optimization ([MIT License](https://github.com/specs-feup/specs-lara/blob/master/LICENSE))

[PB]: http://web.cse.ohio-state.edu/~pouchet.2/software/polybench/ 
[4.2]: https://sourceforge.net/projects/polybench/files/
