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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
void test_expr();
word_t vaddr_read(vaddr_t addr, int len);

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

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  if(nemu_state.state == NEMU_STOP)
		nemu_state.state = NEMU_END;
  return -1;
}

static int cmd_si(char *args) {
	if(args == NULL) 
		cpu_exec(1);
	else
		cpu_exec(atoi(args));
	return 0;
}

static int cmd_info(char *args) {
	if (args != NULL){
		if (strcmp(args, "r") == 0)
			isa_reg_display();
		else if (strcmp(args, "w") == 0)
			wp_display();
		else 
			printf("Unknown argment '%s'\n", args);
	}
	else
		printf("missing args\n");
	return 0;
}		

static int cmd_x(char *args){
  if (args == NULL){
		printf("missing args\n");
		return 0;
	}
	char *args_end = args + strlen(args);
	args = strtok(NULL, " ");
	int print_num = atoi(args);
  char *address = args + strlen(args) + 1; //get arguments address
  if (address >= args_end){
		printf("missing address\n");
		return 0;
	}
	vaddr_t v_addr;
	sscanf(address, "0x%x", &v_addr);
	Log("count = %d address = " FMT_WORD, print_num, v_addr);
	if(v_addr < 0x80000000 || v_addr > 0x87ffffff){
		printf("Out-of-bounds addresses\n");
		return 0;
	}
	for(int i = 0; i < print_num; i++){
		word_t res = vaddr_read(v_addr + i * 4, 4);
		printf("0x%.8x: 0x%.8x\n", v_addr + i * 4, res);
	}
	return 0;
}

static int cmd_p(char *args){
  if (args == NULL){
		printf("missing args\n");
		return 0;
	}
	bool success = true;
	word_t res = expr(args, &success);
	printf("result = %u, hex = 0x%.8x, success = %d\n", res, res, success);
	return 0;
}

static int cmd_w(char *args){
	if(args == NULL){
		printf("missing args\n");
		return 0;
	}
	new_wp(args);
	return 0;
}

static int cmd_d(char *args){
	if(args == NULL){
		printf("missing args\n");
		return 0;
	}
	WP* fpoint = find_wp_head(atoi(args));
	free_wp(fpoint);
	return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
	{ "si", "one step execution could add number", cmd_si},
	{ "info", "print status of r or w", cmd_info},
	{ "x", "scan memory need number and address", cmd_x},
	{ "p", "expression evaluation need expression", cmd_p},
	{ "w", "set a watchpoint need expression", cmd_w},
	{ "d", "delet a watchpoint need id", cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " "); //extract from arg address 
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

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

/* Receive commands from user. */
void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
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
    char *args = cmd + strlen(cmd) + 1; //get arguments address
    if (args >= str_end) {
      args = NULL;  
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; } //cmd true, execute function, input args  
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

	//test_expr();
  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
