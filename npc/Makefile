sim = csrc/main.cpp
ver = vsrc/example.v

all: git
	@verilator -Wall --cc --exe --build $(sim) $(ver)

sim: git
	@verilator -Wall --cc --trace --exe --build $(sim) $(ver)

git:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!


include ../Makefile
