TOPNAME = top
NXDC_FILES = constr/top.nxdc

VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc  \
										-O3 --x-assign fast --x-initial fast --noassert

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)



default: $(BIN) git
$(shell mkdir -p $(BUILD_DIR))


#constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@ 

#poject source
SIM = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")    
VER = $(shell find $(abspath ./vsrc) -name "*.v") 
SIM += $(SRC_AUTO_BIND) 

#rules for NVBorad
include $(NVBOARD_HOME)/scripts/nvboard.mk

#rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
LDFLAGS += -lSDL2 -lSDL2_image  

#bin file
$(BIN): $(VER) $(SIM) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)  
	$(VERILATOR) $(VERILATOR_CFLAGS) \
	--top-module $(TOPNAME) $^ \
	$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
	--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))



all: default 
	@verilator -Wall --cc --exe --build $(SIM) $(VER)


run: $(BIN) git
	@$^
#	@verilator -Wall --cc --trace --exe --build $(SIM) $(VER)

git:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!


include ../Makefile
