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
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_REG, TK_HEX, TK_NUM,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus 43
	{"\\-", '-'},					// sub 45
	{"\\*", '*'},					// 42
	{"/", '/'},						// 47
	{"\\(", '('},					// 40
	{"\\)", ')'},					// 41
  {"==", TK_EQ},				// equal
	//{"s0", TK_REG},				// register
	{"0x[0-9a-f]+", TK_HEX}, // precedence level hexnumber
	{"[0-9]+", TK_NUM},   
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
	  //compile regular expression, then load in re array
		//rules is before compile, re is after
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
	for(int i = 0; i < 32; i++){
		tokens[i].type = 0;
		memset(tokens[i].str, 0, sizeof(tokens[i].str));
	}

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
			//As regexec returned 0, we know we have a match
			//pmatch store the info of matching string, so is the start, eo is the end
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
				//%.*s set printf %s precision as substr_len

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
					case TK_NOTYPE : break;  //notype
					case '+' :
					case '-' :
					case '*' :
					case '/' :
					case '(' :
					case ')' :
					case TK_EQ:
						tokens[nr_token].type = rules[i].token_type;
						Log("Load tokens[%d], type = %d, str = %s",
								nr_token, tokens[nr_token].type, tokens[nr_token].str);
						nr_token++;
						break;
          case TK_NUM:
						tokens[nr_token].type = rules[i].token_type;
						//substr overflow
						if(substr_len >= 32)
							assert(0);
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						tokens[nr_token].str[substr_len] = '\0';
						Log("Load tokens[%d], type = %d, str = %s",
								nr_token, tokens[nr_token].type, tokens[nr_token].str);
						nr_token++;					 
						break;
					case TK_HEX:
						tokens[nr_token].type = rules[i].token_type;
						if(substr_len >= 34)
							assert(0);
						strncpy(tokens[nr_token].str, substr_start + 2, substr_len - 2);
						tokens[nr_token].str[substr_len - 2] = '\0';
						Log("Load tokens[%d], type = %d, str = %s",
								nr_token, tokens[nr_token].type, tokens[nr_token].str);
						nr_token++;					 
						break;
					case TK_REG:
						//read register load in tokens
						break;
					default: ;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q, bool *success){
	if(tokens[p].type != 40 || tokens[q].type != 41){
		return false;
	}
	int top = 0;
	int mintop = 0;
	for(int i = p + 1; i <= q - 1; i++){
		if(tokens[i].type == 40){
			top++;
		}
		if(tokens[i].type == 41){
			top--;
		}
		if(top < mintop){
			mintop = top;
		}
	}
	//(())
	if(top == 0 && mintop == 0){
		//printf("true bracket\n");
		return true;
	}
	//()()
	else if(top == 0 && mintop == -1){
		return false;
	}
	else{
		printf("bracket Bad expression\n");
		*success = false;
		return false;
	} 
}

int prior_level(int type){
	switch (type) {
    case '+':
		case '-': return 1;
    case '*': 
    case '/': return 2;
		default: assert(0);
	}
}

word_t eval(int p, int q, bool *success) {
  if (p > q) {
    /* Bad expression */
		printf("p>q Bad expression\n");
		*success = false;
		return 1;
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
		word_t value;
		if(tokens[p].type == 260){
			sscanf(tokens[p].str, "%u", &value);
		}
		else if(tokens[p].type == 259){
			sscanf(tokens[p].str, "%x", &value);
		}
		//Log("tokens[%d].str = %s, load value = %u", tokens[p].str, value);
		return value;
  }
  else if (check_parentheses(p, q, success) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, success);
  }
  else {
		int op = p;
		int op_type = '*'; 
		int in_bracket = 0;
		word_t val1, val2;
		for(int i = p; i <= q; i++){
			if(tokens[i].type == '('){
				in_bracket++;
			}
			else if(tokens[i].type == ')'){
				in_bracket--;
			}
			else if(in_bracket == 0 && tokens[i].type < 256 && 
							prior_level(tokens[i].type) <= prior_level(op_type)){
				op = i;
				op_type = tokens[i].type;
			}		
		}
    //op = the position of 主运算符 in the token expression;
    val1 = eval(p, op - 1, success);
    val2 = eval(op + 1, q, success);

    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': 
				if(val2 != 0){
					return val1 / val2;
				}
				else{
					printf("/0 error\n");
				}
      default: assert(0);
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
	word_t res;
	res = eval(0, nr_token - 1, success);	
	//Log("result = %u", res);

  return res;
}

void test_expr() {
	//test
		



}
