#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "nemu.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

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
// WP* new_wp() {
//   if (free_ == NULL) {
//     assert(0);
//   }
//   WP *wp = free_;
//   if(head==NULL){
//     free_=free_->next;
//     head=wp;
//     wp->next=NULL;
//     wp->before=NULL;
//   }
//   else{
//     free_ = free_->next;
//     WP *tmp=head;
//     while(tmp->next!=NULL){
//       tmp=tmp->next;
//     }
//     tmp->next=wp;//尾部插入wp
//     wp->before=tmp;//wp的前监视点是tmp
//     wp->next = NULL;
//   }
//   return wp;
// }
WP* new_wp(){
	WP *temp;
	temp = free_;
	free_ = free_->next;
	temp->next = NULL;
	if (head == NULL){
		head = temp;
	} else {
		WP* temp2;
		temp2 = head;
		while (temp2->next != NULL){
			temp2 = temp2->next;
		}
		temp2->next = temp;
	}
	return temp;
}


// void free_wp(WP *wp){
//   //将wp从head删除
//   if(wp->before==NULL){
//     //wp是head
//     head=wp->next;
//   }
//   else if(wp->next==NULL){
//     //wp是最后一个
//     wp->before->next=NULL;
//   }
//   else{
//     wp->before->next=wp->next;
//     wp->next->before=wp->before;
//   }
//   //在free_添加wp
//   WP *cur=free_;
//   if(free_==NULL){
//     free_=wp;
//     wp->next=NULL;
//   }
//   else{
//     while(cur->next!=NULL){
//       cur=cur->next;
//     }
//     cur->next=wp;
//     wp->next=NULL;
//   } 
//}
void free_wp(WP *wp){
	if (wp == NULL){
		assert(0);
	}
	if (wp == head){
		head = head->next;
	} else {
		WP* temp = head;
		while (temp != NULL && temp->next != wp){
			temp = temp->next;
		}
		temp->next = temp->next->next;
	}
	wp->next =free_;
	free_ = wp;
	wp->value = 0;
	wp->exp[0] = '\0';
}

WP* get_wp(int n){
  WP *cur=head;
  while (cur!=NULL)
  {
    if(cur->NO==n){
      return cur;
    }
    cur=cur->next;
  }
  return NULL;
}

// bool check_wp(){
//   WP *tmp=head;
//   bool changed=false;
//   while(tmp!=NULL){
//     bool *success=false;
//     uint32_t new_value=expr(tmp->exp,success);
//     if(new_value!=tmp->value){
//       printf("Watchpoint %d: %s has changed.\n",tmp->NO,tmp->exp);
//       printf("Old Value:0x%08x\n",tmp->value);
//       printf("New Value:0x%08x\n",new_value);
//       tmp->value=new_value;
//       changed=true;
//     }
//     tmp=tmp->next;
//   }
//   return changed;
// }
bool check_wp(){
  printf("触发check\n");
	bool check = false;
	bool *success = false;
	WP *temp = head;
	int expr_temp;
	while(temp != NULL){
		expr_temp = expr(temp->exp, success);
		if (expr_temp != temp->value){
			check = true;
			printf ("Hint watchpoint %d at address 0x%08x\n", temp->NO, cpu.eip);
			temp = temp->next;
			continue;
		}
		printf ("Watchpoint %d: %s\n",temp->NO,temp->exp);
		printf ("Old value = %d\n",temp->value);
		printf ("New value = %d\n",expr_temp);
		temp->value = expr_temp;
		temp = temp->next;
	}
	return check;
}

void display_wp() {
  WP *temp = head;
	if (temp == NULL){
		printf("No watchpoints\n");
	}
	while (temp != NULL){
		printf("Watch point %d: %s\n", temp->NO, temp->exp);
		temp = temp->next;
	}
}


