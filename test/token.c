
#include "test.h"
#include "../src/token.h"
#include "../src/file.h"

#define TOKEN_TEST_INIT(STR) token t; \
    token_init(&t); \
    token_state ts; \
    token_state_init(&ts); \
    char *str = STR;

#define TOKEN_TEST(TYPE, LINE_NO, CHAR_NO, START_POS, END_POS) \
if (t.type != TOKEN_PFX(TYPE) || t.line_no != LINE_NO || t.char_no != CHAR_NO || t.start_pos != START_POS || t.end_pos != END_POS) \
    TEST_FAIL(); \

#define TOKEN_ASSERT(TYPE, LINE_NO, CHAR_NO, START_POS, END_POS) do { \
    if (token_next(&ts, &t, str) != TOKEN_STATUS_PFX(OK)) TEST_FAIL(); \
    TOKEN_TEST(TYPE, LINE_NO, CHAR_NO, START_POS, END_POS) \
} while (0)

#define TOKEN_PEEK_ASSERT(TYPE, LINE_NO, CHAR_NO, START_POS, END_POS) do { \
    if (token_peek(&ts, &t, str) != TOKEN_STATUS_PFX(OK)) TEST_FAIL(); \
    TOKEN_TEST(TYPE, LINE_NO, CHAR_NO, START_POS, END_POS); \
} while (0)

TEST(arith_with_comment) {
    TOKEN_TEST_INIT("abc: 1 + 2 * 3 - 45 / 67 // this is a comment");
    TOKEN_ASSERT(VAR, 1, 1, 0, 2);
    TOKEN_PEEK_ASSERT(ASSIGN, 1, 4, 3, 3);
    TOKEN_ASSERT(ASSIGN, 1, 4, 3, 3);
    TOKEN_ASSERT(INT, 1, 6, 5, 5);
    TOKEN_ASSERT(ADD, 1 , 8, 7, 7);
    TOKEN_ASSERT(INT, 1, 10, 9, 9);
    TOKEN_ASSERT(MUL, 1 , 12, 11, 11);
    TOKEN_ASSERT(INT, 1, 14, 13, 13);
    TOKEN_ASSERT(SUB, 1, 16, 15, 15);
    TOKEN_ASSERT(INT, 1, 18, 17, 18);
    TOKEN_ASSERT(DIV, 1, 21, 20, 20);
    TOKEN_ASSERT(INT, 1, 23, 22, 23);
    TOKEN_PEEK_ASSERT(COMMENT, 1, 26, 25, 44);
    TOKEN_ASSERT(COMMENT, 1, 26, 25, 44);
    TOKEN_ASSERT(END, 1, 46, 45, 45);
}

TEST(define_var_u64) {
    TOKEN_TEST_INIT("usixfour::u64: 12345");
    TOKEN_ASSERT(VAR, 1, 1, 0, 7);
    TOKEN_ASSERT(DEFINE, 1, 9, 8, 9);
    TOKEN_ASSERT(U64, 1, 11, 10, 12);
    TOKEN_ASSERT(ASSIGN, 1, 14, 13, 13);
    TOKEN_ASSERT(INT, 1, 16, 15, 19);
    TOKEN_ASSERT(END, 1, 21, 20, 20);
}

TEST(add_fn_call) {
    TOKEN_TEST_INIT("a: +(1;3 - 2) * 4");
    TOKEN_ASSERT(VAR, 1, 1, 0, 0);
    TOKEN_ASSERT(ASSIGN, 1, 2, 1, 1);
    TOKEN_ASSERT(ADD, 1, 4, 3, 3);
    TOKEN_ASSERT(LPARENS, 1, 5, 4, 4);
    TOKEN_ASSERT(INT, 1, 6, 5, 5);
    TOKEN_ASSERT(SEMICOLON, 1, 7, 6, 6);
    TOKEN_ASSERT(INT, 1, 8, 7, 7);
    TOKEN_ASSERT(SUB, 1, 10, 9, 9);
    TOKEN_ASSERT(INT, 1, 12, 11, 11);
    TOKEN_ASSERT(RPARENS, 1, 13, 12, 12);
    TOKEN_ASSERT(MUL, 1, 15, 14, 14);
    TOKEN_ASSERT(INT, 1, 17, 16, 16);
    TOKEN_ASSERT(END, 1, 18, 17, 17);
}

TEST(negate) {
    TOKEN_TEST_INIT("a: - 1 + 2");
    TOKEN_ASSERT(VAR, 1, 1, 0, 0);
    TOKEN_ASSERT(ASSIGN, 1, 2, 1, 1);
    TOKEN_ASSERT(SUB, 1, 4, 3, 3);
    TOKEN_ASSERT(INT, 1, 6, 5, 5);
    TOKEN_ASSERT(ADD, 1, 8, 7, 7);
    TOKEN_ASSERT(INT, 1, 10, 9, 9);
    TOKEN_ASSERT(END, 1, 11, 10, 10);
}


INIT_TESTS(
    ADD_TEST(arith_with_comment);
    ADD_TEST(define_var_u64);
    ADD_TEST(add_fn_call);
    ADD_TEST(negate);
)
