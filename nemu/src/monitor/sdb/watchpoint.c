/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
		memset(wp_pool[i].expr, 0, sizeof(wp_pool[i].expr));
		wp_pool[i].value = 0;
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(char *args){
	WP *cur = free_;
	if(cur == NULL){
		assert(0);
	}
	free_ = free_->next;
	strcpy(cur->expr, args);
	bool success = true;
	cur->value = expr(args, &success);
	if(success == true){
		printf("new watchpoint id = %d, expr = %s, value = 0x%x, %u\n", 
				cur->NO, cur->expr, cur->value, cur->value);
	}
	cur->next = head;
	head = cur;
	
	return cur;
}

void free_wp(WP *wp){
	WP *pre = head;
	if(pre == NULL){
		assert(0);
	}
	while(pre->next != wp) pre = pre->next;
	pre->next = wp->next;

	wp->next = free_;
	free_ = wp;
}

WP* find_wp_head(int id){
	WP* cur = head;
	while(cur != NULL && cur->NO != id){
		cur = cur->next;
	}
	if(cur == NULL) printf("fail to find\n");
	return cur;
}

void wp_display(){
	WP *cur = head;
	if(cur == NULL){
		printf("nothing watchpoint\n");
	}
	else{
		printf("exist watchpoint list\n");
		while(cur != NULL){
			printf("id = %d, expr = %s, value = 0x%x, %u\n",
					cur->NO, head->expr, cur->value, cur->value);
			cur = cur->next;
		}
	}
}
//watchpoint 
//	bool success;
//	WP *tem = head;
//	word_t pre_value = tem->value;
//	while(tem != NULL){
//		pre_value = tem->value;
//		tem->value = expr(tem->expr, &success);
//		if(tem->value != pre_value){
//			nemu_state.state = NEMU_STOP;
//			printf("watchpoint %d, pre value = %u, next value = %u\n", tem->NO, pre_value, tem->value);
//		}
//	}
