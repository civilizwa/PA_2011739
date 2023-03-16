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
    wp_pool[i].before=&wp_pool[i-1];
    wp_pool[i].exp[0] = '\0';
    wp_pool[i].value = -1;
  }
  wp_pool[0].before=NULL;
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
  if(head==NULL){
    head=wp;
    wp->next=NULL;
    wp->before=NULL;
    free_=free_->next;
  }
  else{
    WP *tmp=head;
    while(tmp->next!=NULL){
      tmp=tmp->next;
    }
    tmp->next=wp;//尾部插入wp
    wp->before=tmp;//wp的前监视点是tmp
    free_ = free_->next;
    wp->next = NULL;
  }
  return wp;
}

void free_wp(WP *wp){
  //将wp从head删除
  wp->before->next=wp->next;
  wp->next->before=wp->before;
  //在free_尾部添加wp
  WP *cur=free_;
  if(free_==NULL){
    free_=wp;
    wp->next=NULL;
  }
  else{
    while(cur->next!=NULL){
      cur=cur->next;
    }
    cur->next=wp;
    wp->next=NULL;
  }
  
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


