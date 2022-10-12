#include <common.h>
#include <stdio.h>    //just for expr

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

//************ epxr read and calculate
  printf("hello");

  FILE *fp;
  uint32_t result;
  char expr[65530];
  if ((fp = fopen("~/workspace/ysyx-workbench/nemu/tools/gen-expr/build/input", "r")) == NULL) printf("\aFile open failed.\n");
  else{
    while(fscanf(fp, "%u%s", &result, expr) == 2)
    {
      printf("%u %s\n", result, expr);

    }

    fclose(fp);
  }

//************ epxr read and calculate

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
