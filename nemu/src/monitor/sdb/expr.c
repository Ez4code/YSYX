#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_NUM, TK_EQ,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   * BOY666: BUT WHY 'Pay attention to the precedence levelS'?
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // multiply
  {"/"  , '/'},         // divide
  {"\\(", '('},         // left bracket
  {"\\)", ')'},         // right bracket
  {"[0-9]+", TK_NUM},   // decimal nums

  {"==", TK_EQ}        // equal
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
          case TK_NOTYPE: break;
          case '+': tokens[nr_token ++].type = '+'; break;
          case '-': tokens[nr_token ++].type = '-'; break;
          case '*': tokens[nr_token ++].type = '*'; break;
          case '/': tokens[nr_token ++].type = '/'; break;
          case '(': tokens[nr_token ++].type = '('; break;
          case ')': tokens[nr_token ++].type = ')'; break;
          case TK_NUM:
            if (substr_len > 32) assert(0);                                //debug
            for(int i = 0; i<substr_len ; i++){
              tokens[nr_token].str[i] = substr_start[i];
            }
            tokens[nr_token ++].type = TK_NUM;
            break;
          default: assert(0); //break;
            //default: TODO();
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

static bool check_parentheses(int p, int q) {
  int i = 0, j = 0;
  for (i = p; i <= q; i++) {
    if (tokens[i].type == '(') j++;
    if (tokens[i].type == ')') j--;
    if (j < 0) return false;        //bad expression
    if (j == 0 && i != p && i != q) return false;
  }
  if(j == 0) return true;
  else return false;                //bad expression
}

static uint32_t eval(int p, int q) {
  if (p > q) {
  /* Bad expression */
  assert(0);                        //debug
  }
  else if (p == q) {
  /* Single token.
   * For now this token should be a number.
   * Return the value of the number.
   */
    return atoi(tokens[p].str);
  }
  else if (check_parentheses(p, q) == true) {
  /* The expression is surrounded by a matched pair of parentheses.
   * If that is the case, just throw away the parentheses.
   */
  return eval(p + 1, q - 1);
  }
  else {
    int i = 0, j = 0;
    int op = 0, op_type = 0;
    int val1, val2;
    bool flag = false;
    for (i = p; i <= q; i++) {
      switch (tokens[i].type) {
        case '(':
          j++;
          break;
        case ')':
          j--;
          break;
        case '+':
        case '-':
          if (!j) {
            op = i;
            op_type = tokens[i].type;
            flag = true;
          }
          break;
        case '*':
        case '/':
          if (!j && !flag) {
            op = i;
            op_type = tokens[i].type;
          }
          break;
        case TK_NUM:
          break;
        default:
          assert(0); //break;
      }
    }
    val1 = eval(p, op - 1);
    val2 = eval(op + 1, q);

    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
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
  else {
    *success = true;
    return eval(0, nr_token);
  }
}
