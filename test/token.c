
#include "token.h"

TEST(arith_with_comment) {
    TOKEN_TEST_INIT("abc: 1 + 2 * 3 - 45 / 67 // this is a comment");
    TOKEN_ASSERT(VAR, 0, 0, 0, 2);
    TOKEN_ASSERT(ASSIGN, 0, 3, 3, 3);
    TOKEN_ASSERT(INT, 0, 5, 5, 5);
    TOKEN_ASSERT(ADD, 0 , 7, 7, 7);
    TOKEN_ASSERT(INT, 0, 9, 9, 9);
    TOKEN_ASSERT(MUL, 0 , 11, 11, 11);
    TOKEN_ASSERT(INT, 0, 13, 13, 13);
    TOKEN_ASSERT(SUB, 0, 15, 15, 15);
    TOKEN_ASSERT(INT, 0, 17, 17, 18);
    TOKEN_ASSERT(DIV, 0, 20, 20, 20);
    TOKEN_ASSERT(INT, 0, 22, 22, 23);
    TOKEN_ASSERT(COMMENT, 0, 25, 25, 44);
    TOKEN_ASSERT(END, 0, 45, 45, 45);
}

TEST(fac_file) {
    char *str;
    file_status status = file_to_c_string("./examples/fac.lang", &str);
    if (status != FILE_STATUS_PFX(OK)) TEST_FAIL();
    free(str);
}

INIT_TESTS(
    ADD_TEST(arith_with_comment);
    ADD_TEST(fac_file);
);
