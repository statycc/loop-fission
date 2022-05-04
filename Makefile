SHELL := /bin/bash

ifndef $CC
CC:=gcc
endif

all: original fission_manual

OPT_LEVELS = O0 O1 O2 O3

SIZES = MINI SMALL MEDIUM LARGE EXTRALARGE

.PHONY: clean
clean:
	rm -rf compiled*/
	rm -rf results/

.NOTPARALLEL:
.PHONY: original
original:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d original -s $(size) -o $(opt); ))


.NOTPARALLEL:
.PHONY: original_autopar
original_autopar:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d original_autopar -s $(size) -o $(opt); ))


.NOTPARALLEL:
.PHONY: fission_manual
fission_manual:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d fission_manual -s $(size) -o $(opt); ))


.NOTPARALLEL:
.PHONY: fission_autopar
fission_autopar:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d fission_autopar -s $(size) -o $(opt); ))
