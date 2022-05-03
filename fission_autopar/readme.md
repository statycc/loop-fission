# Presentation

Those files were obtained by running the source-to-source compiler [AutoPar-Clava](https://dx.doi.org/10.1007/s11227-019-03109-9/)  from [clava](https://github.com/specs-feup/clava) on the following files from the polybench benchmarking suite:

- bicg.c
- fdtd-2d.c 
- gesummv.c
- mvt.c

after they have been "split" by our algorithm, as shared in [fission](../fission/).

# Re-Creating Them

To create those files,

- Install Clava, per their [resources](https://github.com/specs-feup/clava#resources). For anything but windows,
    - Download [their installation script](http://specs.fe.up.pt/tools/clava/clava-update), place it in the folder where you would like to install clava (e.g., `/opt`),
    - Execute the installation script as root.
- run `script_autopar.sh`, after setting the `clara_path` to the right path if it is not `/opt/clava/Clava/Clava.jar`.

This script will automatically, for each .c file,

- Copy the appropriate headers,
- Create the appropriate AutoPar.lara directions (that simply ask to fissionize all the loops in the method function that starts with kernel_),
- Run clava,
- Copy the optimized file to this folder,
- Delete the temporary file.
