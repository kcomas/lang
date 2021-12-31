
#include "token.h"

extern inline void token_init(token *const t);

extern inline void token_state_init(token_state *const ts);

extern inline void token_state_copy(token_state *const dest, const token_state *const src);

static void advance_char_pos(token_state *const ts_tmp) {
    ts_tmp->char_no++;
    ts_tmp->pos++;
}

static char char_at(const token_state *const ts_tmp, const char *const str) {
    return str[ts_tmp->pos];
}

static char char_peek(const token_state *const ts_tmp, const char *const str) {
    return str[ts_tmp->pos + 1];
}

static char char_next(token_state *const ts_tmp, const char *const str) {
    advance_char_pos(ts_tmp);
    return char_at(ts_tmp, str);
}

static token_status load_word(token_state *const ts_tmp, const char *const str) {
    // we are on the fist char of the word
    char c = char_at(ts_tmp, str);
    while (isalpha(c) || isdigit(c)) c = char_next(ts_tmp, str);
    return TOKEN_STATUS_PFX(OK);
}

static token_status load_num(token_state *const ts_tmp, token *const t, const char *const str) {
    // at first digit of num
    char c = char_at(ts_tmp, str);
    while (isdigit(c)) c = char_next(ts_tmp, str);
    // TODO float + other num types
    t->type = TOKEN_PFX(INT);
    return TOKEN_STATUS_PFX(OK);
}

token_status token_get(token_state *const ts, token *const t, const char *const str, bool advance) {
    // do not use token_state passed in for next token
    token_state ts_tmp;
    token_status status;
    token_state_copy(&ts_tmp, ts);
    // on the char of the next token
    char c = char_at(&ts_tmp, str);
    // check if it is a space \s|\t
    while (c == ' ' || c == '\t') c = char_next(&ts_tmp, str);
    // on char of possible valid token
    t->start_pos = ts_tmp.pos;
    t->line_no = ts_tmp.line_no;
    t->char_no = ts_tmp.char_no;
    if (isalpha(c)) {
        if ((status = load_word(&ts_tmp, str)) != TOKEN_STATUS_PFX(OK)) return status;
        t->type = TOKEN_PFX(VAR);
    } else if (isdigit(c)) {
        if ((status = load_num(&ts_tmp, t, str)) != TOKEN_STATUS_PFX(OK)) return status;
    } else {
        switch (c) {
            TOKEN_ONE_CHAR('\0', END);
            case ':':
                c = char_peek(&ts_tmp, str);
                if (c == ':') {
                    t->type = TOKEN_PFX(DEFINE);
                    char_next(&ts_tmp, str);
                } else {
                    t->type = TOKEN_PFX(ASSIGN);
                }
                break;
            TOKEN_ONE_CHAR('+', ADD);
            TOKEN_ONE_CHAR('*', MUL);
            default:
                return TOKEN_STATUS_PFX(INVALID_CHAR);
        }
        c = char_next(&ts_tmp, str);
    }
    // now at first char of next token or space
    t->end_pos = ts_tmp.pos - 1;
    if (advance) token_state_copy(ts, &ts_tmp);
    return TOKEN_STATUS_PFX(OK);
}

extern inline token_status token_next(token_state *const ts, token *const t, const char *const str);

extern inline token_status token_peek(token_state *const ts, token *const t, const char *const str);
