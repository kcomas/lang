
#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define TOKEN_PFX(NAME) TOKEN_##NAME

typedef enum {
    // types
    TOKEN_PFX(VAR),
    TOKEN_PFX(INT),
    TOKEN_PFX(FLOAT),
    TOKEN_PFX(STRING),
    TOKEN_PFX(U1),
    TOKEN_PFX(U8),
    TOKEN_PFX(U16),
    TOKEN_PFX(U32),
    TOKEN_PFX(U64),
    TOKEN_PFX(I8),
    TOKEN_PFX(I16),
    TOKEN_PFX(I32),
    TOKEN_PFX(I64),
    TOKEN_PFX(F32),
    TOKEN_PFX(F64),
    // control constructs
    TOKEN_PFX(COMMENT),
    TOKEN_PFX(END), // null byte of str
    TOKEN_PFX(NEWLINE),
    TOKEN_PFX(SEMICOLON),
    TOKEN_PFX(RETURN),
    TOKEN_PFX(LBRACE), // {
    TOKEN_PFX(RBRACE), // }
    TOKEN_PFX(LBRACKET), // [
    TOKEN_PFX(RBRACKET), // ]
    TOKEN_PFX(LPARENS), // (
    TOKEN_PFX(RPARENS), // )
    TOKEN_PFX(ASSIGN),
    TOKEN_PFX(DEFINE),
    TOKEN_PFX(QUESTION),
    TOKEN_PFX(IF),
    TOKEN_PFX(WHILE),
    // Logic
    TOKEN_PFX(OR),
    TOKEN_PFX(BITOR),
    TOKEN_PFX(EQ),
    TOKEN_PFX(DEEPEQ),
    TOKEN_PFX(LESS),
    // Operators
    TOKEN_PFX(CAST),
    TOKEN_PFX(ADD),
    TOKEN_PFX(SUB),
    TOKEN_PFX(MUL),
    TOKEN_PFX(DIV),
    TOKEN_PFX(READWRITE),
} token_type;

typedef struct {
    token_type type;
    // abs pos in str eg "abc" start = 0 end = 2
    size_t line_no, char_no, start_pos, end_pos; // char_no is at the start of token on that line
} token;

inline void token_init(token *const t) {
    t->type = TOKEN_PFX(END);
    t->line_no = 1;
    t->char_no = 1;
    t->start_pos = 0;
    t->end_pos = 0;
}

#define TOKEN_ONE_CHAR(C, TYPE) case C: \
    t->type = TOKEN_PFX(TYPE); \
    break

#define TOKEN_TYPE_DOUBLE_SWITCH(STR, START, PLUS, C1, C2, TYPE) case C1: \
    switch (STR[START + PLUS]) { \
        TOKEN_ONE_CHAR(C2, TYPE); \
    } \
    break

#define TOKEN_TWO_CHAR(C1, C2, TYPE1, TYPE2) case C1: \
    c = char_peek(&ts_tmp, str); \
    if (c == C2) { \
        t->type = TOKEN_PFX(TYPE2); \
        advance_char_pos(&ts_tmp); \
    } else { \
        t->type = TOKEN_PFX(TYPE1); \
    } \
    break

#define TOKEN_STATUS_PFX(NAME) TOKEN_STATE_##NAME

typedef enum {
    TOKEN_STATUS_PFX(OK),
    TOKEN_STATUS_PFX(INVALID_CHAR),
    TOKEN_STATUS_PFX(INVALID_STRING)
} token_status;

typedef struct {
    size_t line_no, char_no, pos; // pos is abs loc in str
} token_state;

inline void token_state_init(token_state *const ts) {
    ts->line_no = 1;
    ts->char_no = 1;
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
