#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
WP* new_wp();
void free_wp(WP *wp);
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].exp[0] = '\0';
    wp_pool[i].value = -1;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  if (free_ == NULL) {
    assert(0);
  }

  WP *wp = free_;
  free_ = free_->next;
  wp->next = NULL;

  return wp;
}
void free_wp(WP *wp){
  WP *cur=free_;
  while(cur->next!=NULL){
    cur=cur->next;
  }
  cur->next=wp;
  wp->next=NULL;
}
void display_wp() {
  if (head == NULL) {
    printf("There is no watchpoint!\n");
    return ;
  }

  WP *wp;
  printf("NO      expression        value\n");
  wp = head;
  while (wp != NULL) {
    printf("%-5d   %-15s   %-16u\n", wp->NO, wp->exp, wp->value);
    wp = wp->next;
  }
}


