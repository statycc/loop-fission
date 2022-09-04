# Utilities

This directory contains helper files for timing (`polybench-*`, `time_benchmark.sh`, `template-for-new-benchmark.c`).
These are obtained from [PolyBench/C](http://web.cse.ohio-state.edu/~pouchet.2/software/polybench/).

This directory also contains instructions and utilities (`rose.sh`, `dgemvT.C`, `rose_dgemvT.C`) for working with ROSE compiler.

## About ROSE Compiler

We use [ROSE compiler](http://rosecompiler.org/) to automatically transform and parallelize benchmarks, for comparison.
Performing any of these steps requires building the compiler [from source](https://github.com/rose-compiler/rose).
Build instructions are in the [ROSE wiki](https://github.com/rose-compiler/rose/wiki).

After that we apply
the [LoopProcessor](https://github.com/rose-compiler/rose/blob/dab37577feb8eb129c8fc15f6972222c03171c9f/tutorial/LoopProcessor.C)
tool to transform loops,
and [AutoPar](https://github.com/rose-compiler/rose/blob/dab37577feb8eb129c8fc15f6972222c03171c9f/projects/autoParallelization/autoPar.C)
tool to parallelize them.

Note that loop and parallelization transformations are applied to the entire file[^1]. This breaks the
structure expected by the timing utilities and prevents benchmarking the transformed file, unless the original template
code around benchmark kernel is restored. As the last step we extract the kernel transformed by ROSE, and substitute it
back into the original benchmark template.

The rest of this guide details the ROSE compiler operations. It includes examples to help verify the
expected behavior in each step.

### Loop Transformation

**Test program**: [dgemvT.C](https://github.com/rose-compiler/rose/blob/dab37577feb8eb129c8fc15f6972222c03171c9f/tests/roseTests/loopProcessingTests/dgemvT.C)

**Expected result**: [rose_dgemvT.C.save](https://github.com/rose-compiler/rose/blob/dab37577feb8eb129c8fc15f6972222c03171c9f/tests/roseTests/loopProcessingTests/rose_dgemvT.C.save)

**Compilation**

```
/path_to/rose/build/tutorial/loopProcessor --edg:no_warnings -w -c -fs0 -cp 0 /path_to/dgemvT.C
```

**Expected output**

```
array-copy dimension is 0
opt level=0
```

* [LoopProcessor user guide](https://en.wikibooks.org/wiki/ROSE_Compiler_Framework/LoopProcessor)
* [LoopProcessor tests](https://github.com/rose-compiler/rose/tree/b5a170b408bf25c9fdb7170a5de0cb39c6ff0542/tests/roseTests/loopProcessingTests)


### Automatic parallelization

Build the auto-parallelization tool `AutoPar`. Then it can be used to automatically parallelize programs, e.g.:

```
../autoPar --edg:no_warnings -I../headers -c /path_to/program.c
```

There is no expected output for this command.

* [ROSE AutoPar Guide](https://en.wikibooks.org/wiki/ROSE_Compiler_Framework/autoPar)

### Transform all original benchmarks

The following command performs these steps--transformation, parallelization and template restore--on all 
original benchmarks automatically, with the best effort. If it reports errors, you will need to check the output manually.
Under the hood it runs [`rose.sh`](rose.sh).

```
make rose
```

**Expected output**

```
3mm          transformed: ✓   parallel: ✓   restored: ✓
bicg         transformed: ✓   parallel: ✓   restored: ✓
colormap     transformed: ✓   parallel: ✓   restored: ✓
conjgrad     transformed: ✓   parallel: ✓   restored: 🗙
cp50         transformed: ✓   parallel: ✓   restored: ✓
deriche      transformed: ✓   parallel: ✓   restored: ✓
fdtd-2d      transformed: ✓   parallel: ✓   restored: 🗙
gemm         transformed: ✓   parallel: ✓   restored: 🗙
gesummv      transformed: ✓   parallel: ✓   restored: ✓
mvt          transformed: ✓   parallel: ✓   restored: ✓
remap        transformed: 🗙   parallel: 🗙   restored: ✓
tblshft      transformed: ✓   parallel: ✓   restored: ✓
```

The benchmarks with indicated error in last column are caused by removed `scop/endscop` pragma. 
This is not an error in transforming the file, but prevents restoring the templating code. 
It can be resolved by manually by inspecting and replacing the templating code.

Example `remap` fails to transform with following error. This is significant error and prevents measuring this example
in the alternate case. The error message is:

```
array-copy dimension is 0
Error in SymbolicValGenerator::GetSymbolicVal(): unhandled type of binary operator BOP_MOD
lt-loopProcessor: /home/rose/src/src/midend/astUtil/symbolicVal/SymbolicVal.C:344: static SymbolicVal 
  SymbolicValGenerator::GetSymbolicVal(AstInterface&, const AstNodePtr&): Assertion `false' failed.
Aborted (core dumped)
```

There is no known fix at this time. The example is measured as equal to original, due to this transformation failure.


[^1]: [Rose User Manual](http://rosecompiler.org/uploads/ROSE-UserManual.pdf),
15.2.2 Partial Compilation: "At present the transformation does not properly support partial compilation.
This is not a fundamental challenge to the current design, it merely reflects the incomplete state of the project.
See also section 15.4."