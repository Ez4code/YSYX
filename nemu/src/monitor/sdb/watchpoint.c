#include "sdb.h"
#include <memory/vaddr.h>
#include <cpu/cpu.h>



//typedef struct watchpoint {
//  int NO;
//  int value;
//  bool flag;
//  struct watchpoint *next;
//
//  /* TODO: Add more members if necessary */
//
//} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static bool init_flag = false;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
  init_flag = true;
}

/* TODO: Implement the functionality of watchpoint */


WP* new_wp(int value, char * expr){
  if(init_flag == false) init_wp_pool();
  if(free_ == NULL){
    printf("ERROR, wp_pool is full!");
    assert(0);
  }
  WP *new = NULL;
  new = free_;
  free_ = free_->next;
  new->value = value;
  new->next = NULL;
  strcpy(new->str, expr);
  if(head == NULL) head = new;
  else{
    new->next = head;//insert behind
    head = new;
  }
  return head;
}

void free_wp(WP* head, int num){
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
  free_->value = 0;
}

void print_wp(WP* head){
  WP *p = head;
  if(head == NULL){
    printf("ERROR, wp is empty!");
    return;
  }
  while(p != NULL){
//    word_t memory = vaddr_read(p->value, 4);
    printf("WatchPoint: %d %s 0x%08x\n", p->NO, p->str, p->value);
    p = p->next;
  }
  return;
}

void wp_state(){
  WP *p = head;
  if(head == NULL){
    printf("wp is empty!");
    return;
  }
  while(p != NULL){
    bool flag = false;
    bool *success = &flag;
    word_t result = expr(p->str, success);
    if(success){
      if(result != p->value){
        printf("WatchPoint:NO.%d expr:%s value:0x%08x\n", p->NO, p->str, p->value);
        nemu_state.state = NEMU_STOP;
      }
      p = p->next;
    }
  }
  return;
}