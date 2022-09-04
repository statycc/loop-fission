SHELL := /bin/bash

# Making sure no parallelization is performed
.NOTPARALLEL:

# Compiler choice.
# Default: gcc
ifndef $CC
CC:=gcc
endif

# Directory choice.
# Default: original
ifndef $DIR
DIR:=original
endif

# By default, we test original fission and alt
all: clean original fission alt

# Optimization levels
OPT_LEVELS = O0 O1 O2 O3

# Sizes.
# Cf. https://web.cse.ohio-state.edu/~pouchet.2/software/polybench/
SIZES = MINI SMALL MEDIUM LARGE EXTRALARGE

# Rules for folders
.PHONY: original
original:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d original -s $(size) -o $(opt); ))

.PHONY: fission
fission:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d fission -s $(size) -o $(opt); ))

.PHONY: alt
alt:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d alt -s $(size) -o $(opt); ))

# Benchmark specific programs
# Specify DIR argument otherwise it defaults to original
PROGS = 3mm bicg colormap conjgrad cp50 deriche fdtd-2d gemm gesummv mvt remap tblshft

3mm:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p 3mm -s $(size) -o $(opt); ))

bicg:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p bicg -s $(size) -o $(opt); ))

colormap:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p colormap -s $(size) -o $(opt); ))

conjgrad:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p conjgrad -s $(size) -o $(opt); ))

cp50:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p cp50 -s $(size) -o $(opt); ))

deriche:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p deriche -s $(size) -o $(opt); ))

fdtd-2d:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p fdtd-2d -s $(size) -o $(opt); ))

gemm:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p gemm -s $(size) -o $(opt); ))

gesummv:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p gesummv -s $(size) -o $(opt); ))

mvt:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p mvt -s $(size) -o $(opt); ))

remap:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p remap -s $(size) -o $(opt); ))

tblshft:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p tblshft -s $(size) -o $(opt); ))

.PHONY: plots
plots:
	python3 plot.py -d time -f md --millis
	python3 plot.py -d time -f tex --millis
	python3 plot.py -d time -f plot
	python3 plot.py -d speedup -f md --digits 2
	python3 plot.py -d speedup -f tex --digits 2
	python3 plot.py -d speedup -f tex --ss original --st fission
	python3 plot.py -d speedup -f plot

# Cleaning command
clean:
	rm -rf compiled*/
	rm -rf results/
	rm -rf plots/
	rm -rf _*_autopar/
	rm -rf ____tempfile.data.polybench


