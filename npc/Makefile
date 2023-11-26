all:
	@echo "Write this Makefile by your self."
VSRCS = $(shell find $(./vsrc ) -name "*.v")
VERILATOR_FLAGS += --cc --exe
VERILATOR_FLAGS += --trace
VERILATOR_FLAGS += --build

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	verilator $(VERILATOR_FLAGS) $(VSRCS) ./csrc/sim_main.cpp
clean:
	rm -r obj_dir
include ../Makefile
