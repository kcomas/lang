
#include "test.h"
#include "../src/token.h"

#define TOKEN_TEST_INIT(STR) token t; \
    token_init(&t); \
    TOKEN_ASSERT(t, END, 0, 0, 0, 0); \
    token_state ts; \
    token_state_init(&ts); \
    char *str = STR;

#define TOKEN_FOUND() do { \
    if (token_next(&ts, &t, str) != TOKEN_STATUS_PFX(OK)) TEST_FAIL(); \
} while (0)

#define TOKEN_ASSERT(TOKEN, TYPE, LINE_NO, CHAR_NO, START_POS, END_POS) do { \
    if (TOKEN.type != TOKEN_PFX(TYPE) || TOKEN.line_no != LINE_NO || TOKEN.char_no != CHAR_NO || TOKEN.start_pos != START_POS|| TOKEN.end_pos != END_POS) TEST_FAIL(); \
} while (0)

TEST(add_assign) {
    TOKEN_TEST_INIT("abc: 1 + 2 * 3");
    TOKEN_FOUND();
    TOKEN_ASSERT(t, VAR, 0, 0, 0, 2);
    TOKEN_FOUND();
    TOKEN_ASSERT(t, ASSIGN, 0, 3, 3, 3);
    TOKEN_FOUND();
    TOKEN_ASSERT(t, INT, 0, 5, 5, 5);
    TOKEN_FOUND();
    TOKEN_ASSERT(t, ADD, 0 , 7, 7, 7);
    TOKEN_FOUND();
    TOKEN_ASSERT(t, INT, 0 , 9, 9, 9);
    TOKEN_FOUND();
    TOKEN_ASSERT(t, MUL, 0 , 11, 11, 11);
    TOKEN_FOUND();
    TOKEN_ASSERT(t, INT, 0 , 13, 13, 13);
    TOKEN_FOUND();
    TOKEN_ASSERT(t, END, 0 , 14, 14, 14);
}

INIT_TESTS(
    ADD_TEST(add_assign);
);
