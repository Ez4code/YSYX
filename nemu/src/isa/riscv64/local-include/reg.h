#ifndef __RISCV64_REG_H__
#define __RISCV64_REG_H__

#include <common.h>

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < 32));
  return idx;
}

#define gpr(idx) (cpu.gpr[check_reg_idx(idx)])

static inline const char* reg_name(int idx, int width) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}

static inline const int reg_idx(const char *reg_name){
  extern const char* regs[];
  for(int i = 0; i < 32; i ++){
    if(*reg_name == *regs[i]) return i;
  }
  return -1;
}



#endif
