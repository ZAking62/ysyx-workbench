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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static int nr_buf = 0;
static int buf_overflow = 0;
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"#include <stdint.h>\n"
"int main() { "
"  uint32_t result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static void init_buf(){
	memset(buf, 0, sizeof(buf));
	nr_buf = 0;
	buf_overflow = 0;
}

static uint32_t choose(uint32_t n){
	uint32_t rand_num = rand() % n;
	return rand_num;
}

static void gen(char c){
	buf[nr_buf] = c;
	nr_buf++;	
	if(nr_buf >= 65536){
		init_buf();
		buf_overflow = 1;
	}
}

static void gen_space(){
	int max_insert = choose(4);
	while(max_insert--){
		//25% insert space
		if(choose(4) == 0){
			gen(' ');
		}
	}
}

static void gen_rand_op(){
	gen_space();
	char op;
	switch (choose(4)) {
		case 0: op = '+'; break;
		case 1: op = '-'; break;
		case 2: op = '*'; break;
		default: op = '/'; break;
	}
	gen(op);
	gen_space();
}

static void gen_num(){
	uint32_t rand_num = choose(0xfff);
	int len = sprintf(buf + nr_buf, "%u", rand_num);
	nr_buf += len;
	if(nr_buf >= 65536){
		init_buf();
		buf_overflow = 1;
	}
}

static void gen_rand_expr() {
	gen_space();
  switch (choose(3)) {
    case 0: gen_num(); break;
    case 1:
		 	gen_space();
		 	gen('(');
		 	gen_space();
		 	gen_rand_expr();
		 	gen_space();
		 	gen(')');
		 	gen_space();
		 	break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
	gen_space();
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
		init_buf();

    gen_rand_expr();
		
		//filter buf overflow
		if(buf_overflow == 1){
			i--;
			continue;
		}

		buf[nr_buf] = '\0';

    sprintf(code_buf, code_format, buf);

		//write code_buf into /tmp/.code.c
    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

		//call shell command gcc compile, -w ignore warning
		//-Werror filter integer overflow and /0 error
    int ret = system("gcc -Werror -w /tmp/.code.c -o /tmp/.expr");
		//linux return 0 when shell command executes successfully
    if (ret != 0) {
			i--;
			continue;
		}

		//execute command /tmp/.expr, read the command's stdout
    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%u", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
