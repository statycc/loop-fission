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

small: clean sm_eval

# all directories
DIRS = original fission alt

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

sm_eval:
	$(foreach size, SMALL MEDIUM LARGE, $(foreach dir, $(DIRS), $(foreach opt, O0 O1 O2 O3, \
	$(foreach prog, bicg colormap conjgrad deriche fdtd-2d gesummv mvt remap tblshft, \
		./run.sh -c $(CC) -d $(dir) -s $(size) -p $(prog) -o $(opt); ))))

# Benchmark specific programs

3mm:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p 3mm -s $(size) -o $(opt); )))

bicg:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p bicg -s $(size) -o $(opt); )))

colormap:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p colormap -s $(size) -o $(opt); )))

conjgrad:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p conjgrad -s $(size) -o $(opt); )))

cp50:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p cp50 -s $(size) -o $(opt); )))

deriche:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p deriche -s $(size) -o $(opt); )))

fdtd-2d:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p fdtd-2d -s $(size) -o $(opt); )))

gemm:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p gemm -s $(size) -o $(opt); )))

gesummv:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p gesummv -s $(size) -o $(opt); )))

mvt:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p mvt -s $(size) -o $(opt); )))

remap:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p remap -s $(size) -o $(opt); )))

tblshft:
	$(foreach size, $(SIZES), $(foreach dir, $(DIRS), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(dir) -p tblshft -s $(size) -o $(opt); )))

.PHONY: plots
plots:
	python3 plot.py -d time -f md --millis
	python3 plot.py -d time -f tex --millis
	python3 plot.py -d speedup -f md --digits 2
	python3 plot.py -d speedup -f tex --digits 2
	python3 plot.py -d speedup -f plot

# Cleaning command
clean:
	rm -rf compiled*/
	rm -rf eval/results/
	rm -rf eval/plots/
	rm -rf ____tempfile.data.polybench


