/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 *PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 *KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 *NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <memory/paddr.h>
#include <regex.h>
enum {
    TK_NOTYPE = 256,
    EQ,
    RESGISTER,
    NUM,
    HEX,
    OR,
    AND,
    NEQ,
    Negative,
    POINTER,
    /* TODO: Add more token types */

};

static struct rule {
    const char *regex;
    int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE},                 // spaces
    {"\\+", '+'},                      // plus
    {"\\-", '-'},                      // sub
    {"\\*", '*'},                      // mul
    {"\\/", '/'},                      // div
    {"\\(", '('},                      // left bracke
    {"\\)", ')'},                      // right bracke
    {"[0-9]*", NUM},                   // num
    {"\\|\\|", OR},                    // or
    {"\\&\\&", AND},                   // and
    {"\\!\\=", NEQ},                   // not equal
    {"\\=\\=", EQ},                    // equal
    {"\\$[a-zA-Z]*[0-9]*", RESGISTER}, // register
    {"0[xX][0-9a-fA-F]+", HEX},        // hex
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

    for (i = 0; i < NR_REGEX; i++) {
        ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret != 0) {
            regerror(ret, &re[i], error_msg, 128);
            panic("regex compilation failed: %s\n%s", error_msg,
                  rules[i].regex);
        }
    }
}

typedef struct token {
    int type;
    char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e) {
    int position = 0;
    int i;
    regmatch_t pmatch;

    nr_token = 0;

    while (e[position] != '\0') {
        /* Try all rules one by one. */
        for (i = 0; i < NR_REGEX; i++) {
            if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
                pmatch.rm_so == 0) {
                char *substr_start = e + position;
                int substr_len = pmatch.rm_eo;

                Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
                    i, rules[i].regex, position, substr_len, substr_len,
                    substr_start);

                position += substr_len;

                /* TODO: Now a new token is recognized with rules[i]. Add codes
                 * to record the token in the array `tokens'. For certain types
                 * of tokens, some extra actions should be performed.
                 */
                Token tmp_token;
                switch (rules[i].token_type) {
                case TK_NOTYPE:
                    break;
                default:
                    strncpy(tmp_token.str, substr_start, substr_len);
                    tmp_token.str[substr_len] = '\0';
                    tmp_token.type = rules[i].token_type;
                    tokens[nr_token++] = tmp_token;
                    break;
                }
                break;
            }
        }

        if (i == NR_REGEX) {
            printf("no match at position %d\n%s\n%*.s^\n", position, e,
                   position, "");
            return false;
        }
    }

    return true;
}
bool check_parentheses(int p, int q) {
    if (tokens[p].type != '(' || tokens[q].type != ')') {
        return false;
    }
    int l = p, r = q;
    while (l < r) {
        if (tokens[l].type == '(') {
            if (tokens[r].type == ')') {
                l++;
                r--;
                continue;
            } else {
                r--;
            }
        } else if (tokens[l].type == ')') {
            return false;
        } else {
            l++;
        }
    }
    return true;
}
word_t findMajor(word_t p, word_t q) {
    word_t ret = 0;
    word_t par = 0;
    word_t op_type = 0;
    word_t tmp_type = 0;
    for (word_t i = p; i <= q; i++) {
        if (tokens[i].type == '-') {
            if (i == p) {
                tokens[i].type = Negative;
                return i;
            }
        }

        /*        for (int i = 0; i < nr_token; i++) {
                    if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type
           != ')' || tokens[i - 1].type != NUM)) { tokens[i].type = POINTER;
                        return i;
                    }
                }*/
        if (tokens[i].type == NUM) {
            continue;
        } else if (tokens[i].type == '(') {
            par++;
            continue;
        } else if (tokens[i].type == ')') {
            if (par == 0) {
                return -1;
            }
            par--;
        } else if (par > 0) {
            continue;
        } else {
            switch (tokens[i].type) {
            case '*':
            case '/':
                tmp_type = 1;
                break;
            case '+':
            case '-':
                tmp_type = 2;
                break;
            case EQ:
            case NEQ:
                tmp_type = 3;
                break;
            case AND:
                tmp_type = 4;
                break;
            default:
                assert(0);
            }
            if (tmp_type >= op_type) {
                op_type = tmp_type;
                ret = i;
            }
        }
    }
    if (par > 0) {
        return -1;
    }
    return ret;
}
int32_t eval(word_t p, word_t q) {
    if (p > q) {
        assert(0);
        return -1;
    } else if (p == q) {
        return atoi(tokens[p].str);
    } else if (check_parentheses(p, q) == true) {
        return eval(p + 1, q - 1);
    } else {
        word_t op = findMajor(p, q);
        int op_type = tokens[op].type;
        uint32_t val2 = eval(op + 1, q);
        if (tokens[op].type == Negative) {
            val2 = -val2;
            return val2;
        }
        if (tokens[op].type == POINTER) {
            return paddr_read(val2, 4);
        }
        uint32_t val1 = eval(p, op - 1);
        switch (op_type) {
        case '+':
            return val1 + val2;
        case '-':
            return val1 - val2;
        case '*':
            return val1 * val2;
        case '/':
            if (val2 == 0) {
                return true;
            }
            return val1 / val2;
        case EQ:
            return val1 == val2;
        case NEQ:
            return val1 != val2;
        case AND:
            return val1 && val2;
        default:
            assert(0);
        }
    }
    return true;
}
word_t expr(char *e, bool *success) {
    if (!make_token(e)) {
        *success = false;
        return 0;
    }

    /* TODO: Insert codes to evaluate the expression. */
    return eval(0, nr_token - 1);
}
