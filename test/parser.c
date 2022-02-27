
#include "parser.h"

TEST(arith_with_comment) {
    char *str = "abc: 1 + 2 * 3 - 45 / 67 // this is a comment";
    parser_state ps;
    parser_status status;
    parser_state_init(&ps, str);
    parser_node *root;
    if ((status = parser_parse_exp(&ps, &root)) != PARSER_STATUS_PFX(OK)) TEST_FAIL();
    if (ps.tn->type != TOKEN_PFX(END)) TEST_FAIL();
}

INIT_TESTS(
    ADD_TEST(arith_with_comment);
)
