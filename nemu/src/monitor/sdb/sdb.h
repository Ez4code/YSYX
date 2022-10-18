#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);

#define NR_WP 32

typedef struct watchpoint {
    int NO;
    int value;
    int mem;
    bool flag;
    struct watchpoint *next;

    /* TODO: Add more members if necessary */

} WP;

void init_wp_pool();
WP* new_wp(int value);
void free_wp(WP* head, int value);
void print_wp(WP* head);



#endif
