#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <memory/vaddr.h>

enum {
  TK_NOTYPE = 256, TK_HEX, TK_REG, TK_NUM, TK_EQ, TK_NEQ, TK_AND, TK_DEREF

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
  {"0x[a-zA-Z0-9]+", TK_HEX},
  {"\\$+[a-zA-Z0-9]+", TK_REG},
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // multiply
  {"/"  , '/'},         // divide
  {"\\(", '('},         // left bracket
  {"\\)", ')'},         // right bracket
  {"[0-9]+", TK_NUM},   // decimal nums

  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},
  {"&&", TK_AND},
  {"",   TK_DEREF}
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

static Token tokens[65535] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  memset(tokens, 0 ,65535*sizeof(Token));
  bool plus_flag = false;
  bool minus_flag = false;
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
          case '+':
            if(plus_flag || minus_flag) break;
            else{
              tokens[nr_token ++].type = '+';
              plus_flag = true;
              break;
            }
          case '-':
            if(!minus_flag){
              if(plus_flag) tokens[nr_token].type = '-';
              else tokens[nr_token++].type = '-';
              minus_flag = true;
              break;
            }
            else{
              tokens[nr_token - 1].type = '+';
              plus_flag = true;
              break;
            }
          case '*':
            tokens[nr_token ++].type = '*';
            plus_flag = false;
            minus_flag = false;
            break;
          case '/':
            tokens[nr_token ++].type = '/';
            plus_flag = false;
            minus_flag = false;
            break;
          case '(':
            tokens[nr_token ++].type = '(';
            plus_flag = false;
            minus_flag = false;
            break;
          case ')':
            tokens[nr_token ++].type = ')';
            plus_flag = false;
            minus_flag = false;
            break;
          case TK_EQ:
            tokens[nr_token ++].type = TK_EQ;
            plus_flag = false;
            minus_flag = false;
            break;
          case TK_NEQ:
            tokens[nr_token ++].type = TK_EQ;
            plus_flag = false;
            minus_flag = false;
            break;
          case TK_AND:
            tokens[nr_token ++].type = TK_AND;
            plus_flag = false;
            minus_flag = false;
            break;
          case TK_REG:
            if (substr_len > 32) assert(0);                                //debug
            bool flag = false;
            bool *success = &flag;
            uint32_t reg_value = isa_reg_str2val(substr_start+1, success);
            sprintf(tokens[nr_token].str,"%u",reg_value);
            tokens[nr_token ++].type = TK_NUM;
            plus_flag = false;
            minus_flag = false;
            break;
          case TK_HEX:
            if (substr_len > 32) assert(0);                                //debug
            uint32_t hex_value = strtol(substr_start, NULL, 16);
            sprintf(tokens[nr_token].str,"%u",hex_value);
            tokens[nr_token ++].type = TK_NUM;
            plus_flag = false;
            minus_flag = false;
            break;
          case TK_NUM:
            if (substr_len > 32) assert(0);                                //debug
            for(int i = 0; i<substr_len ; i++){
              tokens[nr_token].str[i] = substr_start[i];
            }
            tokens[nr_token ++].type = TK_NUM;
            plus_flag = false;
            minus_flag = false;
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
  bool flag_valid = false;
  for (i = p; i <= q; i++) {
    if (tokens[i].type == '(') {j++; flag_valid = true;}
    if (tokens[i].type == ')') j--;
    if (j < 0) return false;        //bad expression
    if (j == 0 && i != p && i != q) return false;
  }
  if(j == 0 && flag_valid == true) return true;
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
    bool add_flag = false;
    bool bool_flag = false;
    for (i = p; i <= q; i++) {
      switch (tokens[i].type) {
        case '(': j++; break;
        case ')': j--; break;
        case TK_EQ:
        case TK_NEQ:
        case TK_AND:
          if (!j) {
            op = i;
            op_type = tokens[i].type;
            bool_flag = true;
          }
          break;
        case '+':
        case '-':
          if (!j && !bool_flag) {
            op = i;
            op_type = tokens[i].type;
            add_flag = true;
          }
          break;
        case '*':
        case '/':
          if (!j && !bool_flag && !add_flag) {
            op = i;
            op_type = tokens[i].type;
          }
          break;
        case TK_DEREF:
          vaddr_t addr_value = strtol(tokens[i+1].str ,NULL, 10);
          uint32_t mem = vaddr_read(addr_value, 4);
          return mem;
        case TK_NUM: break;
        default: assert(0); //break;
      }
    }
    val1 = eval(p, op - 1);
    val2 = eval(op + 1, q);

    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_AND: return val1 && val2;
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
  *success = true;

  for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0
    || tokens[i - 1].type == '+'
    || tokens[i - 1].type == '-'
    || tokens[i - 1].type == '*'
    || tokens[i - 1].type == '/'
    || tokens[i - 1].type == '('
    || tokens[i - 1].type == TK_REG
    || tokens[i - 1].type == TK_EQ
    || tokens[i - 1].type == TK_NEQ
    || tokens[i - 1].type == TK_AND
  ) ) {
      tokens[i].type = TK_DEREF;
    }
  }

  return eval(0, nr_token-1);

}
