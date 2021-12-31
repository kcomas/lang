
#pragma once

#include "test.h"
#include "../src/token.h"

#define TOKEN_TEST_INIT(STR) token t; \
    token_init(&t); \
    TOKEN_ASSERT(END, 0, 0, 0, 0); \
    token_state ts; \
    token_state_init(&ts); \
    char *str = STR;

#define TOKEN_FOUND() do { \
    if (token_next(&ts, &t, str) != TOKEN_STATUS_PFX(OK)) TEST_FAIL(); \
} while (0)

#define TOKEN_ASSERT(TYPE, LINE_NO, CHAR_NO, START_POS, END_POS) do { \
    if (t.type != TOKEN_PFX(TYPE) || t.line_no != LINE_NO || t.char_no != CHAR_NO || t.start_pos != START_POS|| t.end_pos != END_POS) TEST_FAIL(); \
} while (0)
