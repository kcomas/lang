
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

#define TOKEN_STATE_PFX(NAME) TOKEN_STATE_##NAME

typedef enum {

    TOKEN_STATE_PFX(_LEN)
} token_state_type;

typedef struct {
    token_state_type state;
    size_t file_len, line_no, char_no, pos;
    char *file_str;
} token_state;
