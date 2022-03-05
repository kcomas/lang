
#include <stdarg.h>
#include "test.h"
#include "../src/parser.h"

static parser_node *buf_node(parser_node_type type, const token *const t_ignore, const char *const data) {
    size_t len = strlen(data);
    parser_node_buf *buf = calloc(1, sizeof(parser_node_buf) + sizeof(char) * len  + sizeof(char));
    strcpy(buf->buf, data);
    buf->len = len;
    return parser_node_init(type, t_ignore, (parser_node_data) { .buf = buf });
}

#define BUF_NODE(TYPE, DATA) buf_node(PARSER_NODE_TYPE_PFX(TYPE), &t_ignore, #DATA)

#define TYPE_NODE(TYPE) parser_node_init(PARSER_NODE_TYPE_PFX(TYPE), &t_ignore, (parser_node_data) {})

static parser_node *op_node(parser_node_type type, const token *const t_ignore, parser_node *left, parser_node *right) {
    parser_node *node = parser_node_init(type, t_ignore, (parser_node_data) { .op = parser_node_op_init() });
    node->data.op->left = left;
    node->data.op->right = right;
    return node;
}

static parser_node *call_node(parser_node *const fn, const token *const t_ignore, size_t arg_len, ...) {
    parser_node_call *call = parser_node_call_init(fn);
    va_list args;
    va_start(args, arg_len);
    for (;;) {
        parser_node_list_add_item(&call->args, va_arg(args, parser_node*));
        if (--arg_len == 0) break;
    }
    va_end(args);
    return parser_node_init(PARSER_NODE_TYPE_PFX(CALL), t_ignore, (parser_node_data) { .call = call });
}

#define CALL_NODE(FN, LEN, ARGS...) call_node(FN, &t_ignore, LEN, ARGS)

#define OP_NODE(TYPE, LEFT, RIGHT) op_node(PARSER_NODE_TYPE_PFX(TYPE), &t_ignore, LEFT, RIGHT)

static bool verify_expr(const parser_node *const a, const parser_node *const b);

static bool verify_list(const parser_node_list *const a, const parser_node_list *const b) {
    parser_node_list_item *ah, *bh;
    ah = a->head;
    bh = b->head;
    while (ah != NULL && bh != NULL) {
        if (verify_expr(ah->node, bh->node) == false) return false;
        ah = ah->next;
        bh = bh->next;
        if ((ah == NULL && bh != NULL) || (ah != NULL && bh == NULL)) return false;
    }
    return true;
}

static bool verify_expr(const parser_node *const a, const parser_node *const b) {
    if (a == NULL && b == NULL) return true;
    if (a->type != b->type) return false;
    if (parser_node_type_is_buf(a->type) == true) {
        if (a->data.buf->len == b->data.buf->len && strcmp(a->data.buf->buf, b->data.buf->buf) == 0) return true;
    } else if (parser_node_type_is_type(a->type) == true) {
        return true;
    } else if (a->type == PARSER_NODE_TYPE_PFX(CALL)) {
        if (verify_expr(a->data.call->fn, b->data.call->fn) == false) return false;
        if (verify_list(&a->data.call->args, &b->data.call->args) == false) return false;
        return true;
    } else if (parser_node_type_is_op(a->type) == true) {
        if (verify_expr(a->data.op->left, b->data.op->left) == true && verify_expr(a->data.op->right, a->data.op->right) == true) return true;
    }
    return false;
}

#define PARSER_INIT(PARSER_FN, STR) parser_state ps; \
    token t_ignore; \
    token_init(&t_ignore); \
    parser_node *root = NULL; \
    parser_state_init(&ps, STR); \
    if (PARSER_FN(&ps, &root) != PARSER_STATUS_PFX(OK)) TEST_FAIL(); \
    if (ps.tn.type != TOKEN_PFX(END)) TEST_FAIL()

#define PARSER_TEST_VERIFY(TEST_NODE) if (verify_expr(root, TEST_NODE) == false) TEST_FAIL(); \
    parser_node_free(root); \
    parser_node_free(TEST_NODE)

TEST(arith_with_comment) {
    PARSER_INIT(parser_parse_expr, "abc: 1 + 2 * 3 - 45 / 67 // this is a comment");
    parser_node *d = OP_NODE(DIV, BUF_NODE(INT, 45), BUF_NODE(INT, 67));
    parser_node *mul = OP_NODE(MUL, BUF_NODE(INT, 2), OP_NODE(SUB, BUF_NODE(INT, 3), d));
    parser_node *test = OP_NODE(ASSIGN, BUF_NODE(VAR, abc), OP_NODE(ADD, BUF_NODE(INT, 1), mul));
    PARSER_TEST_VERIFY(test);
}

TEST(define_var_u64) {
    PARSER_INIT(parser_parse_expr, "usixfour::u64: 12345");
    parser_node *test = OP_NODE(DEFINE, BUF_NODE(VAR, usixfour), OP_NODE(ASSIGN, TYPE_NODE(U64), BUF_NODE(INT, 12345)));
    PARSER_TEST_VERIFY(test);
}

TEST(add_fn_call) {
    PARSER_INIT(parser_parse_expr, "a: +(1;3 - 2) * 4");
    parser_node *call = CALL_NODE(OP_NODE(ADD, NULL, NULL), 2, BUF_NODE(INT, 1), OP_NODE(SUB, BUF_NODE(INT, 3), BUF_NODE(INT, 2)));
    parser_node *test = OP_NODE(ASSIGN, BUF_NODE(VAR, a), OP_NODE(MUL, call, BUF_NODE(INT, 4)));
    PARSER_TEST_VERIFY(test);
}

TEST(negate) {
    PARSER_INIT(parser_parse_expr, "a: - 1 + 2");
    parser_node *test = OP_NODE(ASSIGN, BUF_NODE(VAR, a), OP_NODE(SUB, NULL, OP_NODE(ADD, BUF_NODE(INT, 1), BUF_NODE(INT, 2))));
    PARSER_TEST_VERIFY(test);
}

INIT_TESTS(
    ADD_TEST(arith_with_comment);
    ADD_TEST(define_var_u64);
    ADD_TEST(add_fn_call);
    ADD_TEST(negate);
)
