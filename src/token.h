
#pragma once

#include <stdlib.h>
#include <stdbool.h>

#define TOKEN_PFX(NAME) TOKEN_##NAME

typedef enum {
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

#define TOKEN_STATUS_PFX(NAME) TOKEN_STATE_##NAME

typedef enum {
    TOKEN_STATUS_PFX(OK),
    TOKEN_STATUS_PFX(NONE)
} token_status;

typedef struct {
    size_t line_no, char_no, pos;
} token_state;

// str needs to be null terminated
token_status token_get(token_state *const ts, token *const t, char *const str, bool advance);

inline token_status token_next(token_state *const ts, token *const t, char *const str) {
    return token_get(ts, t, str, true);
}

inline token_status token_peek(token_state *const ts, token *const t, char *const str) {
    return token_get(ts, t, str, false);
}
