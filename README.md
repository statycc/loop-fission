# Modified version of Polybench

**The parallel programs are in `parallel/`**

- these are manually transformed programs 
    - Only code block with comment `Main computational kernel....` should be transformed
    - look for `#pragma scop .... #pragma endscop`
    - The rest is templating code
    
- Filename schema:
    - Filename ends with `_og.c` =>  non-parallelizable/same as original version
    - Otherwise: file has been transformed and is parallelizable

**The original benchmark program files are under `original/`**

- these are in a flat directory, unlike original source, but otherwise same

### How to time

We can compare programs in these 2 directories. Script `run.sh` runs [built-in timing benchmark](./utilities/time_benchmark.sh)
on all examples with single command, the timing has been modified to generate tabular results, but is otherwise same 
as what ships with this benchmark suite. 

**Basic usage**

```text
run.sh 
```

Note: use bash shell if on MacOS: `/bin/sh run.sh`
       
**Available arguments**

```
-c  specify which compiler to use                     [default: gcc]
-d  benchmark programs directory (parallel|original)  [default: original]
-o  optimization level e.g O3                         [default: O0] 
-s  skip programs that are not parallelizable         [default: false]
```

### Results

The results can be found in `result` directory. There are two files per run: 

- `[timestamp]_model.txt`: machine+processor snapshot
- `[timestamp].txt`: actual results of timing
    - attribute labels: `program, variance (%), time (s)`
    - labels are not include in the file


- timing options are default: 5 runs, take avg 3 runs (not min, max)
- dataset size defaults to `STANDARD_DATASET`
- if variance is >= 5% should repeat the timing

Timing all examples without parallelization takes ~ 30-40 minutes.
Timing parallelizable examples only take ~ 3 minutes.

* * *

### Original source

* [Polybench/C benchmark @ Ohio State](http://web.cse.ohio-state.edu/~pouchet.2/software/polybench/)
* [Benchmark readme](./README)