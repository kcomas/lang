
#pragma once

#include <stdlib.h>
#include <stdbool.h>

#define TOKEN_PFX(NAME) TOKEN_##NAME

typedef enum {
    TOKEN_PFX(END), // null byte of str
    TOKEN_PFX(NEWLINE),
    TOKEN_PFX(SEMICOLON),
    TOKEN_PFX(VAR),
    TOKEN_PFX(INT),
    TOKEN_PFX(FLOAT),
    TOKEN_PFX(ASSIGN),
    TOKEN_PFX(DEFINE),
    TOKEN_PFX(ADD),
    TOKEN_PFX(_LEN)
} token_type;

typedef struct {
    token_type type;
    // pos abs position of first char of token in string
    // len abs len eg abc len = 3
    size_t line_no, char_no, pos, len;
} token;

inline void token_init(token *const t) {
    t->type = TOKEN_PFX(END);
    t->line_no = 0;
    t->char_no = 0;
    t->pos = 0;
    t->len = 0;
}

#define TOKEN_STATUS_PFX(NAME) TOKEN_STATE_##NAME

typedef enum {
    TOKEN_STATUS_PFX(OK)
} token_status;

typedef struct {
    size_t line_no, char_no, pos;
} token_state;

inline void token_state_init(token_state *const ts) {
    ts->line_no = 0;
    ts->char_no = 0;
    ts->pos = 0;
}

inline void token_state_copy(token_state *const dest, const token_state *const src) {
    dest->line_no = src->line_no;
    dest->char_no = src->char_no;
    dest->pos = src->pos;
}

// str needs to be null terminated
token_status token_get(token_state *const ts, token *const t, const char *const str, bool advance);

inline token_status token_next(token_state *const ts, token *const t, const char *const str) {
    return token_get(ts, t, str, true);
}

inline token_status token_peek(token_state *const ts, token *const t, const char *const str) {
    return token_get(ts, t, str, false);
}
