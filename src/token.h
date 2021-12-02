
#pragma once

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define TOKEN_PFX(NAME) TOKEN_##NAME

typedef enum {

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
    TOKEN_STATUS_PFX(OK)
} token_status_type;

typedef struct {
    token_status status;
    size_t len, line_no, char_no, pos;
    char *str;
} token_state;

inline token_state *token_state_init(size_t len, char *str) {
    token_state *ts = calloc(1, sizeof(token_state));
    ts->len = len;
    ts->str = str;
    return ts;
}
