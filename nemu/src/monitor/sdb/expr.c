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
    if (tokens[p].type != '(' && tokens[q].type != ')') {
        return false;
    }
    int l = p, r = q;
    while (l < q) {
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
        } else
            l++;
    }
    return true;
}
int32_t eval(word_t p, word_t q) {
    if (p > q) {

    } else if (p == q) {

    } else if (check_parent(p, q) == true) {

    } else {
    }
}
word_t expr(char *e, bool *success) {
    if (!make_token(e)) {
        *success = false;
        return 0;
    }

    /* TODO: Insert codes to evaluate the expression. */
    // return eval(0, nr_token - 1);
    return 0;
}
