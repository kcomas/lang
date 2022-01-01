
#include "token.h"

TEST(add_assign) {
    TOKEN_TEST_INIT("abc: 1 + 2 * 3");
    TOKEN_ASSERT(VAR, 0, 0, 0, 2);
    TOKEN_ASSERT(ASSIGN, 0, 3, 3, 3);
    TOKEN_ASSERT(INT, 0, 5, 5, 5);
    TOKEN_ASSERT(ADD, 0 , 7, 7, 7);
    TOKEN_ASSERT(INT, 0, 9, 9, 9);
    TOKEN_ASSERT(MUL, 0 , 11, 11, 11);
    TOKEN_ASSERT(INT, 0, 13, 13, 13);
    TOKEN_ASSERT(END, 0 , 14, 14, 14);
}

INIT_TESTS(
    ADD_TEST(add_assign);
);
