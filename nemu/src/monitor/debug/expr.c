#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#define BAD_EXP -1111

enum {
  TK_NOTYPE = 256, TK_EQ,NUM, ADD, MINUS, MULTIPLY, DIVIDE, 
  LBRACKET, RBRACKET, REG, HEX,AND, OR, NEQ,NEG

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  {"0[xX][0-9a-fA-F]+", HEX},   // hex number
  {"[0-9]+", NUM},      // numbers
  {"\\-", MINUS},       // minus
  {"\\*", MULTIPLY},    // multiply
  {"\\/", DIVIDE},      // divide
  {"\\(", LBRACKET},    // left bracket
  {"\\)", RBRACKET},    // right bracket
  {"\\$e[abc]x", REG},  // register
  {"\\$e[bs]p", REG},
  {"\\$e[sd]i", REG},
  {"\\$eip", REG},
  {"&&", AND},          // and
  {"\\|\\|", OR},       // or
  {"!=", NEQ},           // not equal
  {"!",'!'}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
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

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case NUM:
          case REG:
          case HEX: 
           for (i = 0; i < substr_len; i++)
              tokens[nr_token].str[i] = substr_start[i];
            tokens[nr_token].str[i] = '\0';
            nr_token++;
            break;
          case ADD:
          case MINUS:
          case MULTIPLY:
          case DIVIDE:
          case LBRACKET:
          case RBRACKET:
            tokens[nr_token].str[0] = substr_start[0];
            tokens[nr_token++].str[1] = '\0';
            break;
          case AND:
          case OR:
          case TK_EQ:
          case NEQ:
            tokens[nr_token].str[0] = substr_start[0];
            tokens[nr_token].str[1] = substr_start[1];
            tokens[nr_token++].str[2] = '\0';
            break;
          default: TODO();
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
bool check_parentheses(int p, int q);
int find_dominant_operator(int p, int q);
int priority(int i);
uint32_t eval(int p,int q);
bool judge_exp();
uint32_t getnum(char str);

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  if(!judge_exp()){
    *success=false;
    return 0;
  }
  if(tokens[0].type=='-'){
    tokens[0].type=NEG;
  }
  for(int i=1;i<nr_token;i++){
    if(tokens[i].type=='-'){
      if(tokens[i-1].type!=NUM&&tokens[i-1].type!=')'){
        tokens[i].type=NEG;
      }
    }
  }
  *success=true;
  return eval(0,nr_token-1);
}

bool check_parentheses(int p, int q) {
  int i, bra = 0;//bra用于匹配左右括号

  for (i = p; i <= q; i++) {
    if (tokens[i].type == LBRACKET) {
      bra++;//出现左括号，bra+1
    }
    if (tokens[i].type == RBRACKET) {
      bra--;//出现右括号,bra-1
    }
    if(bra == 0 && i < q) {
      return false;
    }
  }

  return true;
}

int find_dominant_operator(int p, int q) {
  int i = 0, j, cnt;
  int op = 0, opp, pos = -1;
  for (i = p; i <= q; i++){
    if (tokens[i].type == NUM || tokens[i].type == REG || tokens[i].type == HEX)
      continue;
    else if (tokens[i].type == LBRACKET) {
      cnt = 0;
      for (j = i + 1; j <= q; j++) {
        if (tokens[j].type == RBRACKET) {
          cnt++;
          i += cnt;
          break;
        }
        else
          cnt++;
      }
    }
    else {
      opp = priority(i);
      if (opp >= op) {
        pos = i;
        op = opp;
      }
    }
  }
//  printf("op = %d, pos = %d\n",  op, pos);
  return pos;
}

int priority(int i) {
  if(tokens[i].type ==NEG||tokens[i].type =='!') return 1;
  if (tokens[i].type == MULTIPLY || tokens[i].type == DIVIDE) return 2;
  else if (tokens[i].type == ADD || tokens[i].type == MINUS) return 3;
  else if (tokens[i].type == NEQ || tokens[i].type == TK_EQ) return 4;
  else if (tokens[i].type == OR) return 5;
  else if (tokens[i].type == AND) return 6;
  
  return 0;
}

uint32_t eval(int p, int q) {
//  printf("in the eval p = %d, q = %d\n", p, q);
  if (p > q){
    return BAD_EXP;
  }
  else if (p == q){
    if (tokens[p].type == NUM) {
      return atoi(tokens[p].str);
    }
    else if (tokens[p].type == REG) {
      if (strcmp(tokens[p].str, "$eax") == 0){  
        //printf("eax = %u\n", cpu.eax);
        return cpu.eax;}
      else if (strcmp(tokens[p].str, "$ebx") == 0)  return cpu.ebx;
      else if (strcmp(tokens[p].str, "$ecx") == 0)  return cpu.ecx;
      else if (strcmp(tokens[p].str, "$edx") == 0)  return cpu.edx;
      else if (strcmp(tokens[p].str, "$ebp") == 0)  return cpu.ebp;
      else if (strcmp(tokens[p].str, "$esp") == 0)  return cpu.esp;
      else if (strcmp(tokens[p].str, "$esi") == 0)  return cpu.esi;
      else if (strcmp(tokens[p].str, "$edi") == 0)  return cpu.edi;
      else if (strcmp(tokens[p].str, "$eip") == 0)  return cpu.eip;
    }
    else if (tokens[p].type == HEX) {
      int cnt, i, len, sum = 0;
      len = strlen(tokens[p].str);
      cnt = 1;

      for (i = len-1; i >= 0; i--) {
        sum = sum + cnt * getnum(tokens[p].str[i]);
        cnt *= 16;
      }
      return sum;
    }
  }
  else if (check_parentheses(p, q)){
    return eval(p + 1, q - 1);
  }
  else {
    int op = find_dominant_operator(p, q);
    int result;
    switch(tokens[op].type){
      case NEG:
      return -eval(p+1,q);
      case '!':
      result=eval(p+1,q);
      if(result!=0) return 0;
      else return 1;
    }
    //printf("op = %d\n", op);
    uint32_t val1 = eval(p, op - 1);
    uint32_t val2 = eval(op + 1, q);
    //printf("op = %d val1 = %u val2 = %u\n", op, val1, val2);

    switch (tokens[op].type) {
      case ADD:
        return val1 + val2;
      case MINUS:
        return val1 - val2;
      case MULTIPLY:
        return val1 * val2;
      case DIVIDE:
        return val1 / val2;
      case AND:
        return val1 && val2;
      case OR:
        return val1 || val2;
      case TK_EQ:
        return val1 == val2;
      case NEQ:
        return val1 != val2;
      default:
        assert(0);
    }
  }
  return 1;
}

bool judge_exp() {
  int i, cnt;
  
  cnt = 0;
  for (i = 0; i <= nr_token; i++) {
    if (tokens[i].type == LBRACKET)
      cnt++;
    else if (tokens[i].type == RBRACKET)
      cnt--;

    if (cnt < 0)
      return false;
  }

  return true;
}

uint32_t getnum(char str)
{
  if (str >= '0' && str <= '9') 
    return str - '0';
  else if (str >= 'a' && str <= 'f') 
    return str - 'a' + 10;
  else if (str >= 'A' && str <= 'F') 
    return str - 'A' + 10;
  return 0;
}