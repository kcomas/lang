
#include "token.h"

extern inline void token_init(token *const t);

extern inline void token_state_init(token_state *const ts);

extern inline void token_state_copy(token_state *const dest, const token_state *const src);

static void advance_char_pos(token_state *const ts) {
    ts->char_no++;
    ts->pos++;
}

static char char_at(token_state *const ts, const char *const str) {
    return str[ts->pos];
}

static char char_next(token_state *const ts, const char *const str) {
    advance_char_pos(ts);
    return char_at(ts, str);
}

token_status token_get(token_state *const ts, token *const t, const char *const str, bool advance) {
    // do not use token_state passed in for next token
    token_state ts_tmp;
    token_state_copy(&ts_tmp, ts);
    // on the char of the next token
    char c = char_at(&ts_tmp, str);
    // check if it is a space \s\t
    while (c == ' ' || c == '\t') c = char_next(&ts_tmp, str);
    size_t start_pos = ts_tmp.pos;
    size_t end_pos = start_pos;
    if (advance) {
        // update new loc in str
        ts_tmp.pos = end_pos;
        token_state_copy(ts, &ts_tmp);
    }
    return TOKEN_STATUS_PFX(OK);
}

extern inline token_status token_next(token_state *const ts, token *const t, const char *const str);

extern inline token_status token_peek(token_state *const ts, token *const t, const char *const str);
