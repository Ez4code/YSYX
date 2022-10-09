#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static bool divisor = false;
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static void gen_num(){
  uint32_t n;
  char num[32];
  if(divisor){
    do{ n = rand(); n = n + rand(); }while(n != 0);
  }
  else: {
    n = rand();        //even if RAND_MAX is less than 2147483647.
    n = n + rand();
  }
  sprintf(num,"%ld",n);
  strcpy(buf,num);
}

static void gen_rand_op() {
  switch (choose(3)){
    case 0: strcpy(buf,"\\+"); break;
    case 1: strcpy(buf,"\\-"); break;
    case 2: strcpy(buf,"\\*"); break;
    default: strcpy(buf,"/"); divisor = true; break;
  }
}

static void gen_rand_expr() {
  switch (choose(3)) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); divisor = false;break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int len = 0;
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
    len = strlen(buf);
    if(len >= 65530) break;
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
