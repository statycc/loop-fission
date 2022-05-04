SHELL := /bin/bash

ifndef $CC
CC:=gcc
endif

ifndef $DIR
DIR:=original
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
.PHONY: original_autopar_edited
original_autopar_edited:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d original_autopar_edited -s $(size) -o $(opt); ))

.NOTPARALLEL:
.PHONY: fission_manual
fission_manual:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d fission_manual -s $(size) -o $(opt); ))


.NOTPARALLEL:
.PHONY: fission_autopar
fission_autopar:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d fission_autopar -s $(size) -o $(opt); ))


# benchmark specific programs
# specify DIR argument otherwise it defaults to original

.NOTPARALLEL:
3mm:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p 3mm -s $(size) -o $(opt); ))

.NOTPARALLEL:
bicg:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p bicg -s $(size) -o $(opt); ))

.NOTPARALLEL:
deriche:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p deriche -s $(size) -o $(opt); ))

.NOTPARALLEL:
fdtd-2d:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p fdtd-2d -s $(size) -o $(opt); ))

.NOTPARALLEL:
getsummv:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p getsummv -s $(size) -o $(opt); ))

.NOTPARALLEL:
mvt:
	 $(foreach size, $(SIZES), $(foreach opt, $(OPT_LEVELS), ./run.sh -c $(CC) -d $(DIR) -p mvt -s $(size) -o $(opt); ))
