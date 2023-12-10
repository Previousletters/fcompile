VLIB = vlib.exe
VLOG = vlog.exe
VSIM = vsim.exe
TB_NAME = testbench_conv_mp
DEFINES = 
RUN_DIR = ${shell pwd}
RTL_DIR = ${RUN_DIR}/../install/rtl
INC_DIR = ${RUN_DIR}/../install/include
TBS_DIR = ${RUN_DIR}/../install/tb
TOP_MOD  = ${RTL_DIR}/00_TOP
MCIF_MOD = ${RTL_DIR}/01_MCIF
CONV_MOD = ${RTL_DIR}/02_CONV
OTHERS   = ${RTL_DIR}/03_others
MAT_MOD  = ${RTL_DIR}/04_Matrix
AXI_MOD  = ${TBS_DIR}/AXI
VLOG_F1  = $(wildcard ${TOP_MOD}/*.v ${MCIF_MOD}/*.v ${OTHERS}/*.v)
VLOG_F2  = $(wildcard ${CONV_MOD}/*.v ${CONV_MOD}/*/*.v)
VLOG_F3  = $(wildcard ${MAT_MOD}/ACT/*.v ${MAT_MOD}/LN/*.v ${MAT_MOD}/LN/*/*.v)
VLOG_F4  = $(wildcard ${MAT_MOD}/Concat/*.v ${MAT_MOD}/others/*.v ${MAT_MOD}/Softmax/*.v)
VLOG_F5  = $(wildcard ${MAT_MOD}/Transpose/*.v ${MAT_MOD}/*.v ${AXI_MOD}/*.sv)
TB_FILE  = ${TBS_DIR}/${TB_NAME}.sv


compile:
	${VLIB} work
	${VLOG} ${VLOG_F1} +incdir+${INC_DIR}
	${VLOG} ${VLOG_F2} +incdir+${INC_DIR}
	${VLOG} ${VLOG_F3} +incdir+${INC_DIR}
	${VLOG} ${VLOG_F4} +incdir+${INC_DIR}
	${VLOG} ${VLOG_F5} +incdir+${INC_DIR}


sim:
	${VLOG} ${TB_FILE} ${DEFINES} +incdir+${INC_DIR}
	${VSIM} -c -L work ${TB_NAME} -do "run -all; quit"

.PHONY: compile sim