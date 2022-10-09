#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

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

size_t my_strlcpy(char *des, const char *src, size_t length) {
  size_t ret = strlen(src);
  if(des[length-1] == '\0') length--;
  for (int i = 0; src[i] != '\0' && i <= ret - 1; i++){
    des[length+i] = src[i];
  }
  des[ret] = '\0';
  return strlen(src);
}

uint32_t choose(uint32_t n){
  return rand() % n;
}

static void gen(char str){
  strncpy(buf+strlen(buf),&str,1);
}

static void gen_num(){
  uint32_t n;
  char num[32];
  if(divisor){
    do{ n = rand(); n = n + rand(); }while(n != 0);
  }
  else {
    n = rand();        //even if RAND_MAX is less than 2147483647.
    n = n + rand();
  }
  sprintf(num,"%u",n);
  strncpy(buf+strlen(buf),num,12);
}

static void gen_rand_op() {
  char str;
  switch (choose(3)){
    case 0:  str = '+'; break;
    case 1:  str = '-'; break;
    case 2:  str = '*'; break;
    default: str = '/'; divisor = true; break;
  }
  strncpy(buf+strlen(buf), &str, 1);
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
    memset(buf,0,sizeof(buf));
    gen_rand_expr();
    len = strlen(buf);
    if(len >= 65530) continue;
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
    if(fscanf(fp, "%d", &result)){
      pclose(fp);
      printf("%u %s\n", result, buf);
    };
  }
  return 0;
}
