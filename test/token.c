
#include "test.h"
#include "../src/token.h"

#define TOKEN_FOUND(STATUS) do \
    if (STATUS != TOKEN_STATUS_PFX(OK)) TEST_FAIL(); \
while (0)

#define TOKEN_ASSERT(TOKEN, TYPE, LINE_NO, CHAR_NO, START_POS, END_POS) do { \
    if (TOKEN.type != TOKEN_PFX(TYPE) || TOKEN.line_no != LINE_NO || TOKEN.char_no != CHAR_NO || TOKEN.start_pos != START_POS|| TOKEN.end_pos != END_POS) TEST_FAIL(); \
} while (0)

TEST(add_assign) {
    token t;
    token_init(&t);
    TOKEN_ASSERT(t, END, 0, 0, 0, 0);
    token_state ts;
    token_state_init(&ts);
    char *str = "abc: 1 + 2 * 3";
}

INIT_TESTS(
    ADD_TEST(add_assign);
);
