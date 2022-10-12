#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif



//************ epxr read and calculate ************
  printf("hello");
#include <stdio.h>    //just for expr
#include "monitor/sdb/sdb.h"      //just for expr
  FILE *fp;
  uint32_t true_result;
  uint32_t result;
  char expression[65530];
  if ((fp = fopen("/home/boy666/workspace/ysyx-workbench/nemu/tools/gen-expr/build/input", "r")) == NULL) printf("\aFile open failed.\n");
  else{
    while(fscanf(fp, "%u%s", &true_result, expression) == 2)
    {
      printf("%u %s\n", true_result, expression);
      bool flag = false;
      bool *success = &flag;
      result = expr(expression, success);

      if(success){
        if(result == true_result)
        printf("result = good\n");

      }
    }

    fclose(fp);
  }

//************ epxr read and calculate ************





  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
