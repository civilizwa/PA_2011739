#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,NUM, ADD, MINUS, MULTIPLY, DIVIDE, 
  LBRACKET, RBRACKET, REG, HEX,AND, OR, NEQ

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
  {"!=", NEQ}           // not equal
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

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

  return 0;
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
    if(bra == 0 && i < q) {//若bra==0，输出true
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