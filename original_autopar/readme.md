# Presentation

Those files were obtained by running the source-to-source compiler [AutoPar-Clava](https://dx.doi.org/10.1007/s11227-019-03109-9/)  from [clava](https://github.com/specs-feup/clava) on the following 6 files from the polybench benchmarking suite:

- 3mm.c
- bicg.c
- fdtd-2d.c 
- gesummv.c
- mvt.c

after they have been annotated with `pragma` directives, as shared in [original_annotated](../original_annotated/).

The results were checked an found equivalent to the files produced by the authors at <https://zenodo.org/record/1889368>.

# Re-Creating Them

To create those files,

- Install Clava, per their [resources](https://github.com/specs-feup/clava#resources). For anything but windows,
    - Download [their installation script](http://specs.fe.up.pt/tools/clava/clava-update), place it in the folder where you would like to install clava (e.g., `/opt`),
    - Execute the installation script as root.
- run `script_autopar.sh`.

This script will automatically, for each .c file,

- Copy the appropriate headers into [original_annotated](../original_annotated/),
- Create the appropriate CMakeLists.txt file,
- Create the appropriate AutoPar.lara directions (that simply ask to parallelize the loops that have a `pragma` directive before them),
- Run clava, using CMake,
- Copy the optimized file to this folder,
- Delete the temporary file.
