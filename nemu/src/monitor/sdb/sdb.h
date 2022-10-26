#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);

#define NR_WP 32

typedef struct watchpoint {
    /* TODO: Add more members if necessary */
    int NO;
    int value;
    bool flag;
    char str[32];
    struct watchpoint *next;

} WP;

void init_wp_pool();
WP* new_wp(int value, char * expr);
void free_wp(WP* head, int value);
void print_wp(WP* head);
void wp_state();



#endif
