#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args, char *) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args, char *) {
//  return -1;
	printf("Exit NEMU\n");
	exit(0);
}

static int cmd_help(char *args, char *);

static int cmd_si(char *args, char *){
    cpu_exec(atoi(args));
    return 0;
}

static int cmd_info(char *args, char *);

static int cmd_x(char * args, char * sub_args){
//    printf("*args %s, *str_end  %s",*args, *sub_args);
    printf("1:%s   2:%s",args,sub_args);
    //word_t vaddr_read(vaddr_t addr, 4);
    return 0;
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *, char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","Let the program execute N instructions in a single step "
         "\n     and then suspend the execution. N default value is 1",cmd_si},
  { "info", "'info r' print regs state\n       'info w' print watchpoint information", cmd_info},
  { "x", "Calculate the value of the expression EXPR,"
         "\n    take the result as the starting memory address, "
         "\n    and output N consecutive 4 bytes in hexadecimal form", cmd_x}

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args, char *) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_info(char *args, char *){
    if(*args == 'r')isa_reg_display();
    if(*args == 'w');
    else printf("Unknown sub command '%s'\n", args);
    return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL, NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = strtok(NULL, " ");
    char *sub_args = NULL;

      //char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }
    else
        sub_args = strtok(NULL, " ");

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args, sub_args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
