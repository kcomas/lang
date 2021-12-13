
#include "test.h"
#include "../src/token.h"

#define TOKEN_FOUND(TOKEN_STATE) do \
    if (TOKEN_STATE->status != TOKEN_STATUS_PFX(OK)) TEST_FAIL(); \
while (0)

#define TOKEN_ASSERT(TOKEN, TYPE, LINE_NO, CHAR_NO, POS, LEN) do { \
    if (TOKEN.type != TOKEN_PFX(TYPE) || TOKEN.line_no != LINE_NO || TOKEN.char_no != CHAR_NO || TOKEN.pos != POS || TOKEN.len != LEN) TEST_FAIL(); \
} while (0)

TEST(add_assign) {
    token t;
    token_state *ts = token_state_init();
    TOKEN_FOUND(ts);
    char *str = "a: 1 + 2";
    token_state_free(ts);
}

INIT_TESTS(
    ADD_TEST(add_assign);
);
