
#include "test.h"
#include "../src/parser.h"

static parser_node *op_node(parser_node_type type, const token *const ignore_t, parser_node *left, parser_node *right) {
    parser_node *node = parser_node_init(type, ignore_t, (parser_node_data) { .op = parser_node_op_init() });
    node->data.op->left = left;
    node->data.op->right = right;
    return node;
}

#define OP_NODE(TYPE, LEFT, RIGHT) op_node(PARSER_NODE_TYPE_PFX(TYPE), &ignore_t, LEFT, RIGHT)

static parser_node *buf_node(parser_node_type type, const token *const ignore_t, const char *const data) {
    size_t len = strlen(data);
    parser_node_buf *buf = calloc(1, sizeof(parser_node_buf) + sizeof(char) * len  + sizeof(char));
    strcpy(buf->buf, data);
    buf->len = len;
    return parser_node_init(type, ignore_t, (parser_node_data) { .buf = buf });
}

#define BUF_NODE(TYPE, DATA) buf_node(PARSER_NODE_TYPE_PFX(TYPE), &ignore_t, #DATA)

#define TYPE_NODE(TYPE) parser_node_init(PARSER_NODE_TYPE_PFX(TYPE), &ignore_t, (parser_node_data) {})

static bool verify_expr(const parser_node *const target, const parser_node *const test) {
    if (target == NULL && test == NULL) return true;
    if (target->type != test->type) return false;
    if (parser_node_type_is_op(target->type) == true) {
        if (verify_expr(target->data.op->left, test->data.op->left) == true && verify_expr(target->data.op->right, target->data.op->right) == true) return true;
    } else if (parser_node_type_is_buf(target->type) == true) {
        if (target->data.buf->len == test->data.buf->len && strcmp(target->data.buf->buf, test->data.buf->buf) == 0) return true;
    } else if (parser_node_type_is_type(target->type) == true) {
        return true;
    }
    return false;
}

#define PARSER_TEST_INIT(PARSER_FN, STR) parser_state ps; \
    token ignore_t; \
    token_init(&ignore_t); \
    parser_node *root = NULL; \
    parser_state_init(&ps, STR); \
    if (PARSER_FN(&ps, &root) != PARSER_STATUS_PFX(OK)) TEST_FAIL(); \
    if (ps.tn.type != TOKEN_PFX(END)) TEST_FAIL()

#define PARSER_TEST_VERIFY(TEST_NODE) if (verify_expr(root, test) == false) TEST_FAIL(); \
    parser_node_free(root); \
    parser_node_free(TEST_NODE)

TEST(arith_with_comment) {
    PARSER_TEST_INIT(parser_parse_exp, "abc: 1 + 2 * 3 - 45 / 67 // this is a comment");
    parser_node *d = OP_NODE(DIV, BUF_NODE(INT, 45), BUF_NODE(INT, 67));
    parser_node *mul = OP_NODE(MUL, BUF_NODE(INT, 2), OP_NODE(SUB, BUF_NODE(INT, 3), d));
    parser_node *test = OP_NODE(ASSIGN, BUF_NODE(VAR, abc), OP_NODE(ADD, BUF_NODE(INT, 1), mul));
    PARSER_TEST_VERIFY(test);
}

TEST(define_var_u64) {
    PARSER_TEST_INIT(parser_parse_exp, "usixfour::u64: 12345");
    parser_node *test = OP_NODE(DEFINE, BUF_NODE(VAR, usixfour), OP_NODE(ASSIGN, TYPE_NODE(U64), BUF_NODE(INT, 12345)));
    PARSER_TEST_VERIFY(test);
}

INIT_TESTS(
    ADD_TEST(arith_with_comment);
    ADD_TEST(define_var_u64);
)
