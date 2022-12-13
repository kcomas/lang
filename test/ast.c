
#include "test.h"
#include "../src/ast.h"

TEST(arith_with_comment) {
    ast_state as;
    if (ast_state_init(&as, parser_parse_expr, "abc: 1 + 2 * 3 - 45 / 67 // this is a comment") != AST_STATUS_PFX(OK)) TEST_FAIL();
    printf("\n%d\n", as.p_status);
}

INIT_TESTS(
    ADD_TEST(arith_with_comment);
);
