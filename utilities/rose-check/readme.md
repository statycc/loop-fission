# LoopProcessor check 

Running the [LoopProcessor](https://github.com/rose-compiler/rose/blob/dab37577feb8eb129c8fc15f6972222c03171c9f/tutorial/LoopProcessor.C) 
requires building the [ROSE compiler](https://github.com/rose-compiler/rose) from source.
Build instructions are in the [wiki](https://github.com/rose-compiler/rose/wiki). 

After building, perform this simple sanity check to ensure ROSE compiler's LoopProcessor is producing expected results.

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

### Related

* [LoopProcessor user guide](https://en.wikibooks.org/wiki/ROSE_Compiler_Framework/LoopProcessor)
* [LoopProcessor tests](https://github.com/rose-compiler/rose/tree/b5a170b408bf25c9fdb7170a5de0cb39c6ff0542/tests/roseTests/loopProcessingTests)
