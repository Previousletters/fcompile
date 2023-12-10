TOP_MODULE = testbench_HBM_MVM
SIM_DEFINE = 
LIB_PATH = work

$(LIB_PATH):
	vlib.exe $@

run : $(LIB_PATH)
	vlog.exe $(TOP_MODULE).sv $(SIM_DEFINE) +incdir+.
	vsim.exe -c -L work $(TOP_MODULE) -do "run -all; quit"

all: run

.PHONY: run all
