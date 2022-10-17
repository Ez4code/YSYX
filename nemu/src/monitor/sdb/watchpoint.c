#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  int value;
  bool flag;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].flag = false;
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


WP* new_wp(int value){
  if(free_ == NULL){
    printf("ERROR, wp_pool is full!");
    assert(0);
  }
  WP *new = NULL;
  new = free_;
  free_ = free_->next;
  new->value = value;
  new->next = NULL;
  if(head == NULL) head = new;
  else{
    new->next = head;//insert behind
    head = new;
  }
  return head;
}

void free_wp(int num){
  if(head == NULL){
    printf("ERROR, wp_pool is empty");
    return;
  }
  WP *p = head;
  while(p->NO != num){
    p = p->next;
    if (p->next == NULL){
      printf("ERROR, No %d watchpoint!",num);
      return;
    }
  }
  free_ = p;
  free_->next = free_;
}

void print_wp(){
  WP *p = head;
  if(head == NULL){
    printf("ERROR, wp is empty!");
    return;
  }
  while(p != NULL){
    printf("%d  0x%08x\n", p->NO, p->value);
    p = p->next;
  }
  return;

}