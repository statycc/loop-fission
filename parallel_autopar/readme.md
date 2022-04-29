# Presentation

Those files were obtained by running the source-to-source compiler [AutoPar-Clava](https://dx.doi.org/10.1007/s11227-019-03109-9/)  from [clava](https://github.com/specs-feup/clava) on the following 6 files from the polybench benchmarking suite:

- bicg.c
- fdtd-2d.c 
- gesummv.c
- jacobi-1d.c 
- jacobi-2d.c 
- mvt.c

after they have been "split" by our algorithm, and annotated with `pragma` directives, as shared in [parallel](../parallel/).

# Re-Creating Them

To create those files,

- Install Clava, per their [resources](https://github.com/specs-feup/clava#resources). For anything but windows,
    - Download [their installation script](http://specs.fe.up.pt/tools/clava/clava-update), place it in the folder where you would like to install clava (e.g., `/opt`),
    - Execute the installation script as root.
- run `script_autopar.sh`.

This script will automatically, for each .c file,

- Copy the appropriate headers into [parallel](../parallel/),
- Create the appropriate CMakeLists.txt file,
- Create the appropriate AutoPar.lara directions (that simply ask to parallelize the loops that have a `pragma` directive before them),
- Run clava, using CMake,
- Copy the optimized file to this folder,
- Delete the temporary file.
