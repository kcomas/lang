
#include "test.h"
#include "../src/parser.h"

#define NODE_LIST(NODES...) ((parser_node*[]) {NODES})

#define ADD_TO_LIST(LIST_TGT, LEN, LIST)  do { \
    for (size_t i = 0; i < LEN; i++) parser_node_list_add_item(LIST_TGT, LIST[i]); \
} while(0)

static parser_node *buf_node(parser_node_type type, const token *const t_ignore, const char *const data) {
    size_t len = strlen(data);
    parser_node_buf *buf = calloc(1, sizeof(parser_node_buf) + sizeof(char) * len  + sizeof(char));
    strcpy(buf->buf, data);
    buf->len = len;
    return parser_node_init(type, t_ignore, (parser_node_data) { .buf = buf });
}

#define BUF_NODE(TYPE, DATA) buf_node(PARSER_NODE_TYPE_PFX(TYPE), &t_ignore, #DATA)

#define TYPE_NODE(TYPE) parser_node_init(PARSER_NODE_TYPE_PFX(TYPE), &t_ignore, (parser_node_data) {})

static parser_node *fn_node(const token *const t_ignore, size_t arg_len, parser_node *restrict const args[], size_t body_len, parser_node *restrict const body[]) {
    parser_node_fn *fn = parser_node_fn_init();
    ADD_TO_LIST(&fn->args, arg_len, args);
    ADD_TO_LIST(&fn->body, body_len, body);
    return parser_node_init(PARSER_NODE_TYPE_PFX(FN), t_ignore, (parser_node_data) { .fn = fn });
}

#define FN_NODE(ARG_LEN, ARGS, BODY_LEN, BODY) fn_node(&t_ignore, ARG_LEN, ARGS, BODY_LEN, BODY)

static parser_node *list_node(parser_node_type type, const token *const t_ignore, size_t item_len, parser_node *const items[]) {
    parser_node_list *list = parser_node_list_init();
    ADD_TO_LIST(list, item_len, items);
    return parser_node_init(type, t_ignore, (parser_node_data) { .list = list });
}

#define LIST_NODE(TYPE, ITEM_LEN, ITEMS) list_node(PARSER_NODE_TYPE_PFX(TYPE), &t_ignore, ITEM_LEN, ITEMS);

static parser_node *get_node(parser_node_type type, const token *const t_ignore, parser_node *const tgt, size_t arg_len, parser_node *const args[]) {
    parser_node_get *get = parser_node_get_init(tgt);
    ADD_TO_LIST(&get->args, arg_len, args);
    return parser_node_init(type, t_ignore, (parser_node_data) { .get = get });
}

#define GET_NODE(TYPE, FN, LEN, ARGS) get_node(PARSER_NODE_TYPE_PFX(TYPE), &t_ignore, FN, LEN, ARGS)

static parser_node *op_node(parser_node_type type, const token *const t_ignore, parser_node *restrict const left, parser_node *restrict const right) {
    parser_node *node = parser_node_init(type, t_ignore, (parser_node_data) { .op = parser_node_op_init() });
    node->data.op->left = left;
    node->data.op->right = right;
    return node;
}

#define OP_NODE(TYPE, LEFT, RIGHT) op_node(PARSER_NODE_TYPE_PFX(TYPE), &t_ignore, LEFT, RIGHT)

static bool verify_expr(const parser_node *restrict const a, const parser_node *restrict const b);

static bool verify_list(const parser_node_list *restrict const a, const parser_node_list *restrict const b) {
    parser_node_list_item *ah, *bh;
    ah = a->head;
    bh = b->head;
    while (ah != NULL && bh != NULL) {
        if (!verify_expr(ah->node, bh->node)) return false;
        ah = ah->next;
        bh = bh->next;
        if ((ah == NULL && bh != NULL) || (ah != NULL && bh == NULL)) return false;
    }
    return true;
}

static bool verify_expr(const parser_node *restrict const a, const parser_node *restrict const b) {
    if (a == NULL && b == NULL) return true;
    if (a == NULL || b == NULL || a->type != b->type) return false;
    if (parser_node_type_is_buf(a->type)) return a->data.buf->len == b->data.buf->len && strcmp(a->data.buf->buf, b->data.buf->buf) == 0;
    else if (parser_node_type_is_type(a->type)) return true;
    else if (a->type == PARSER_NODE_TYPE_PFX(FN)) return verify_list(&a->data.fn->args, &b->data.fn->args) && verify_list(&a->data.fn->body, &b->data.fn->body);
    else if (parser_node_type_is_list(a->type)) return verify_list(a->data.list, b->data.list);
    else if (parser_node_type_is_get(a->type)) return verify_expr(a->data.get->tgt, b->data.get->tgt) && verify_list(&a->data.get->args, &b->data.get->args);
    else if (parser_node_type_is_op(a->type)) return verify_expr(a->data.op->left, b->data.op->left) && verify_expr(a->data.op->right, b->data.op->right);
    return false;
}

#define PARSER_TEST_INIT(PARSER_FN, STR) parser_state ps; \
    token t_ignore; \
    token_init(&t_ignore); \
    parser_node *root = NULL; \
    parser_state_init(&ps, STR); \
    if (PARSER_FN(&ps, &root) != PARSER_STATUS_PFX(OK)) TEST_FAIL(); \
    if (ps.tn.type != TOKEN_PFX(END)) TEST_FAIL()

#define PARSER_TEST_VERIFY(TEST_NODE) if (!verify_expr(root, TEST_NODE)) TEST_FAIL(); \
    parser_node_free(root); \
    parser_node_free(TEST_NODE)

TEST(arith_with_comment) {
    PARSER_TEST_INIT(parser_parse_expr, "abc: 1 + 2 * 3 - 45 / 67 // this is a comment");
    parser_node *d = OP_NODE(DIV, BUF_NODE(INT, 45), BUF_NODE(INT, 67));
    parser_node *mul = OP_NODE(MUL, BUF_NODE(INT, 2), OP_NODE(SUB, BUF_NODE(INT, 3), d));
    parser_node *test = OP_NODE(ASSIGN, BUF_NODE(VAR, abc), OP_NODE(ADD, BUF_NODE(INT, 1), mul));
    PARSER_TEST_VERIFY(test);
}

TEST(define_var_u64) {
    PARSER_TEST_INIT(parser_parse_expr, "u64::usixfour: 12345");
    parser_node *test = OP_NODE(ASSIGN, OP_NODE(DEFINE, TYPE_NODE(U64), BUF_NODE(VAR, usixfour)), BUF_NODE(INT, 12345));
    PARSER_TEST_VERIFY(test);
}

TEST(add_fn_call) {
    PARSER_TEST_INIT(parser_parse_expr, "a: +(1;3 * 2) - 4");
    parser_node *call = GET_NODE(CALL, OP_NODE(ADD, NULL, NULL), 2, NODE_LIST(BUF_NODE(INT, 1), OP_NODE(MUL, BUF_NODE(INT, 3), BUF_NODE(INT, 2))));
    parser_node *test = OP_NODE(ASSIGN, BUF_NODE(VAR, a), OP_NODE(SUB, call, BUF_NODE(INT, 4)));
    PARSER_TEST_VERIFY(test);
}

TEST(negate) {
    PARSER_TEST_INIT(parser_parse_expr, "a: - 1 + 2");
    parser_node *test = OP_NODE(ASSIGN, BUF_NODE(VAR, a), OP_NODE(SUB, NULL, OP_NODE(ADD, BUF_NODE(INT, 1), BUF_NODE(INT, 2))));
    PARSER_TEST_VERIFY(test);
}

TEST(fn_direct_call) {
    PARSER_TEST_INIT(parser_parse_expr, "{(u64::a;u64::b;u64) a ** b }(3;2)");
    parser_node *args[] = { OP_NODE(DEFINE, TYPE_NODE(U64), BUF_NODE(VAR, a)), OP_NODE(DEFINE, TYPE_NODE(U64), BUF_NODE(VAR, b)), TYPE_NODE(U64) };
    parser_node *fn = FN_NODE(3, args, 1, NODE_LIST(OP_NODE(EXP, BUF_NODE(VAR, a), BUF_NODE(VAR, b))));
    parser_node *test = GET_NODE(CALL, fn, 2, NODE_LIST(BUF_NODE(INT, 3), BUF_NODE(INT, 2)));
    PARSER_TEST_VERIFY(test);
}

TEST(vec_direct_index) {
    PARSER_TEST_INIT(parser_parse_expr, "a: 12 + [1; 2; 3][2]");
    parser_node *vec = LIST_NODE(VEC, 3, NODE_LIST(BUF_NODE(INT, 1), BUF_NODE(INT, 2), BUF_NODE(INT, 3)));
    parser_node *index = GET_NODE(INDEX, vec, 1, NODE_LIST(BUF_NODE(INT, 2)));
    parser_node *test = OP_NODE(ASSIGN, BUF_NODE(VAR, a), OP_NODE(ADD, BUF_NODE(INT, 12), index));
    PARSER_TEST_VERIFY(test);
}

TEST(basic_multiline) {
    PARSER_TEST_INIT(parser_parser_mod, "a: 1\nb: a + 2");
    parser_node *a = OP_NODE(ASSIGN, BUF_NODE(VAR, a), BUF_NODE(INT, 1));
    parser_node *b = OP_NODE(ASSIGN, BUF_NODE(VAR, b), OP_NODE(ADD, BUF_NODE(VAR, a), BUF_NODE(INT, 2)));
    parser_node *test = LIST_NODE(MOD, 2, NODE_LIST(a, b));
    PARSER_TEST_VERIFY(test);
}

INIT_TESTS(
    ADD_TEST(arith_with_comment);
    ADD_TEST(define_var_u64);
    ADD_TEST(add_fn_call);
    ADD_TEST(negate);
    ADD_TEST(fn_direct_call);
    ADD_TEST(vec_direct_index);
    ADD_TEST(basic_multiline);
)
