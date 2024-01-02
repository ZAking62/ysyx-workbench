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
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

uint32_t choose(uint32_t n){
	uint32_t rand_num = rand() % n;
	return rand_num;
}

static void gen(char bracket){
	buf[nr_buf] = bracket;
	//printf("write bracket : %c\n", buf[nr_buf]);
	nr_buf++;	
}

static void gen_rand_op(){
	char op;
	switch (choose(4)) {
		case 0: op = '+'; break;
		case 1: op = '-'; break;
		case 2: op = '*'; break;
		default: op = '/'; break;
	}
	buf[nr_buf] = op;
	//printf("write op: %c\n", buf[nr_buf]);
	nr_buf++;
}

static void gen_num(){
	uint32_t rand_num = choose(0xff) + 1;
	int len = sprintf(buf + nr_buf, "%u", rand_num);
	//printf("write : %s\n", buf + nr_buf);
	nr_buf += len;
}

static void gen_rand_expr() {
  switch (choose(3)) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
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
		memset(buf, 0, sizeof(buf));
		nr_buf = 0;

    gen_rand_expr();
		buf[nr_buf] = '\0';

    sprintf(code_buf, code_format, buf);

		//write code_buf into /tmp/.code.c
    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

		//call shell command gcc compile, -w ignore warning
		//linux return 0 when shell command executes successfully
    int ret = system("gcc -w /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

		//execute command /tmp/.expr, read the command's stdout
    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

		//load stdout into result
    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);
		if(ret != 1){
			i--;
			continue;
		}

    printf("result = %u, buf = %s\n", result, buf);
  }
  return 0;
}
