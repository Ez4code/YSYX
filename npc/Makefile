sim = csrc/main.cpp
ver = vsrc/example.v

all:
	@verilator -Wall --cc --exe --build $(sim) $(ver)

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@verilator -Wall --cc --trace --exe --build $(sim) $(ver)


include ../Makefile
