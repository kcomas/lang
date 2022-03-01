
#include "test.h"
#include "../src/parser.h"

static parser_node *op_node(parser_node_type type, parser_node *left, parser_node *right) {
    parser_node *node = parser_node_init(type, (parser_node_data) { .op = parser_node_op_init() });
    node->data.op->left = left;
    node->data.op->right = right;
    return node;
}

#define OP_NODE(TYPE, LEFT, RIGHT) op_node(PARSER_NODE_TYPE_PFX(TYPE), LEFT, RIGHT)

static parser_node *buf_node(parser_node_type type, const char *const data) {
    parser_node_buf *buf = calloc(1, sizeof(parser_node_buf) + sizeof(char) * strlen(data) + sizeof(char));
    strcpy(buf->buf, data);
    return parser_node_init(type, (parser_node_data) { .buf = buf });
}

#define BUF_NODE(TYPE, DATA) buf_node(PARSER_NODE_TYPE_PFX(TYPE), #DATA)

static bool verify_expr(const parser_node *const target, const parser_node *const test) {
    if (target == NULL && test == NULL) return true;
    if (target->type != test->type) return false;
    if (parser_node_is_op(target) == true) {
        if (verify_expr(target->data.op->left, test->data.op->left) == false || verify_expr(target->data.op->right, target->data.op->right) == false) return false;
    } else if (parser_node_is_buf(target) == true) {
        if (strcmp(target->data.buf->buf, test->data.buf->buf) !=0) return false;
    } else {
        return false;
    }
    return true;
}

TEST(arith_with_comment) {
    char *str = "abc: 1 + 2 * 3 - 45 / 67 // this is a comment";
    parser_state ps;
    parser_status status;
    parser_state_init(&ps, str);
    parser_node *root;
    if ((status = parser_parse_exp(&ps, &root)) != PARSER_STATUS_PFX(OK)) TEST_FAIL();
    if (ps.tn->type != TOKEN_PFX(END)) TEST_FAIL();
    parser_node *d = OP_NODE(DIV, BUF_NODE(INT, 45), BUF_NODE(INT, 67));
    parser_node *mul = OP_NODE(MUL, BUF_NODE(INT, 2), OP_NODE(SUB, BUF_NODE(INT, 3), d));
    parser_node *assign = OP_NODE(ASSIGN, BUF_NODE(VAR, abc), OP_NODE(ADD, BUF_NODE(INT, 1), mul));
    if (verify_expr(root, assign) == false) TEST_FAIL();
}

INIT_TESTS(
    ADD_TEST(arith_with_comment);
)
