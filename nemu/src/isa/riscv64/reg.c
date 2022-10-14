#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
    for(int idx = 0; idx <= 31; idx++){
        printf("%-15s0x%-18lx%-24ld\n", regs[idx], gpr(idx), gpr(idx));
    }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  int idx = reg_idx(s);
  if(idx == -1) *success = false;
  return gpr(idx);
//  printf("%-15s0x%-18lx%-24ld\n", regs[idx], gpr(idx), gpr(idx));
//  return 0;
}
