TB_NAME = testbench_conv_mp
PWD_DIR = $(shell pwd)
SRC_DIR = ${PWD_DIR}/../rtl
INC_DIR = ${PWD_DIR}/../include
TBS_DIR = ${PWD_DIR}/../tb
INS_DIR = ${PWD_DIR}/install
RUN_DIR = ${PWD_DIR}/run
DEFINES = 
SIM_TOOL = ModelSIM

all: 

install: 
	mkdir $(RUN_DIR)
	mkdir $(INS_DIR)
	cp -r $(SRC_DIR) $(INS_DIR)
	cp -r $(INC_DIR) $(INS_DIR)
	cp -r $(TBS_DIR) $(INS_DIR)
	ln -s $(PWD_DIR)/bin/run.${SIM_TOOL} $(RUN_DIR)/Makefile

clean:
	rm -r $(RUN_DIR)
	rm -r $(INS_DIR)

compile:
	make RUN_DIR=${RUN_DIR} compile -C ${RUN_DIR}

sim:
	make RUN_DIR=${RUN_DIR} TB_NAME=${TB_NAME} DEFINES=${DEFINES} sim -C ${RUN_DIR}

.PHONY: install
