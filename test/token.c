
#include "token.h"

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

TEST(fac_file) {
    char *file_str;
    file_status status = file_to_c_string("./examples/fac.lang", &file_str);
    if (status != FILE_STATUS_PFX(OK)) TEST_FAIL();
    TOKEN_TEST_INIT(file_str);
    TOKEN_ASSERT(NEWLINE, 1, 1, 0, 0);
    TOKEN_ASSERT(COMMENT, 2, 1, 1, 12);
    TOKEN_ASSERT(NEWLINE, 2, 13, 13, 13);
    TOKEN_ASSERT(VAR, 3, 1, 14, 16);
    TOKEN_ASSERT(ASSIGN, 3, 4, 17, 17);
    TOKEN_ASSERT(LBRACE, 3, 6, 19, 19);
    TOKEN_ASSERT(LPARENS, 3, 7, 20, 20);
    TOKEN_ASSERT(VAR, 3, 8, 21, 21);
    TOKEN_ASSERT(DEFINE, 3, 9, 22, 23);
    TOKEN_ASSERT(U64, 3, 11, 24, 26);
    TOKEN_ASSERT(RPARENS, 3, 14, 27, 27);
    TOKEN_ASSERT(LBRACKET, 3, 15, 28, 28);
    TOKEN_ASSERT(U64, 3, 16, 29, 31);
    TOKEN_ASSERT(RBRACKET, 3, 19, 32, 32);
    TOKEN_ASSERT(NEWLINE, 3, 20, 33, 33);
    TOKEN_ASSERT(RETURN, 4, 5, 38, 39);
    TOKEN_ASSERT(IF, 4, 8, 41, 42);
    TOKEN_ASSERT(LBRACE, 4, 11, 44, 44);
    TOKEN_ASSERT(NEWLINE, 4, 12, 45, 45);
    TOKEN_ASSERT(LPARENS, 5, 9, 54, 54);
    TOKEN_ASSERT(OR, 5, 10, 55, 55);
    TOKEN_ASSERT(LPARENS, 5, 11, 56, 56);
    TOKEN_ASSERT(VAR, 5, 12, 57, 57);
    TOKEN_ASSERT(EQ, 5, 14, 59, 59);
    TOKEN_ASSERT(INT, 5, 16, 61, 61);
    TOKEN_ASSERT(SEMICOLON, 5, 17, 62, 62);
    TOKEN_ASSERT(VAR, 5, 19, 64, 64);
    TOKEN_ASSERT(EQ, 5, 21, 66, 66);
    TOKEN_ASSERT(INT, 5, 23, 68, 68);
    TOKEN_ASSERT(RPARENS, 5, 24, 69, 69);
    TOKEN_ASSERT(RPARENS, 5, 25, 70, 70);
    TOKEN_ASSERT(LBRACE, 5, 27, 72, 72);
    TOKEN_ASSERT(INT, 5, 29, 74, 74);
    TOKEN_ASSERT(RBRACE, 5, 31, 76, 76);
    TOKEN_ASSERT(NEWLINE, 5, 32, 77, 77);
    TOKEN_ASSERT(LBRACE, 6, 9, 86, 86);
    TOKEN_ASSERT(VAR, 6, 11, 88, 88);
    TOKEN_ASSERT(MUL, 6, 13, 90, 90);
    TOKEN_ASSERT(VAR, 6, 15, 92, 94);
    TOKEN_ASSERT(LPARENS, 6, 18, 95, 95);
    TOKEN_ASSERT(VAR, 6, 19, 96, 96);
    TOKEN_ASSERT(SUB, 6, 21, 98, 98);
    TOKEN_ASSERT(INT, 6, 23, 100, 100);
    TOKEN_ASSERT(RPARENS, 6, 24, 101, 101);
    TOKEN_ASSERT(RBRACE, 6, 26, 103, 103);
    TOKEN_ASSERT(NEWLINE, 6, 27, 104, 104);
    TOKEN_ASSERT(RBRACE, 7, 5, 109, 109);
    TOKEN_ASSERT(NEWLINE, 7, 6, 110, 110);
    TOKEN_ASSERT(RBRACE, 8, 1, 111, 111);
    TOKEN_ASSERT(NEWLINE, 8, 2, 112, 112);
    TOKEN_ASSERT(NEWLINE, 9, 1, 113, 113);
    TOKEN_ASSERT(INT, 10, 1, 114, 114);
    TOKEN_ASSERT(READWRITE, 10, 3, 116, 117);
    TOKEN_ASSERT(CAST, 10, 6, 119, 119);
    TOKEN_ASSERT(STRING, 10, 7, 120, 121);
    TOKEN_ASSERT(CAST, 10, 9, 122, 122);
    TOKEN_ASSERT(VAR, 10, 11, 124, 126);
    TOKEN_ASSERT(LPARENS, 10, 14, 127, 127);
    TOKEN_ASSERT(INT, 10, 15, 128, 129);
    TOKEN_ASSERT(RPARENS, 10, 17, 130, 130);
    TOKEN_ASSERT(NEWLINE, 10, 18, 131, 131);
    TOKEN_ASSERT(INT, 11, 1, 132, 132);
    TOKEN_ASSERT(READWRITE, 11, 3, 134, 135);
    TOKEN_ASSERT(STRING, 11, 6, 137, 140);
    TOKEN_ASSERT(NEWLINE, 11, 10, 141, 141);
    TOKEN_ASSERT(END, 12, 1, 142, 142);
    free(str);
}

INIT_TESTS(
    ADD_TEST(arith_with_comment);
    ADD_TEST(fac_file);
)
