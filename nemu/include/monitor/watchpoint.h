#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  struct watchpoint *before;
  /* TODO: Add more members if necessary */
  char exp[100];
  uint32_t value;

} WP;

WP* new_wp();
void free_wp(WP *wp);
bool check_wp();
#endif
