
#include "parser.h"

extern inline bool parser_node_type_is_buf(parser_node_type type);

extern inline bool parser_node_type_is_op(parser_node_type type);

extern inline void parser_node_list_add_item(parser_node_list *const nl, const parser_node *const node);

extern inline parser_node_buf *parser_node_buf_init(const token *const t, const char *const str);

extern inline void parser_node_buf_free(parser_node_buf *buf);

// previous node must be null to stop to bufs from being next to each other
#define TOKEN_TO_BUF(TYPE) if (*node != NULL) return PARSER_STATUS_PFX(NODE_FOR_BUF_NOT_NULL); \
    *node = parser_node_init(PARSER_NODE_TYPE_PFX(TYPE), (parser_node_data) { .buf = parser_node_buf_init(&ps->tn, ps->str) }); \
    return parser_parse_exp(ps, node)

#define TOKEN_TO_BUF_CASE(TYPE) case TOKEN_PFX(TYPE): \
    TOKEN_TO_BUF(TYPE)

extern inline parser_node_op *parser_node_op_init(void);

extern inline void parser_node_op_free(parser_node_op *op);

#define TOKEN_TO_OP(TYPE) case TOKEN_PFX(TYPE): \
    tmp_node = parser_node_init(PARSER_NODE_TYPE_PFX(TYPE), (parser_node_data) { .op = parser_node_op_init() }); \
    tmp_node->data.op->left = *node; \
    *node = tmp_node; \
    return parser_parse_exp(ps, &tmp_node->data.op->right)

extern inline parser_node_fn *parser_node_fn_init(void);

extern inline parser_node *parser_node_init(parser_node_type type, parser_node_data data);

void parser_node_free(parser_node *node) {
    if (node == NULL) return;
    switch (node->type) {
        case  PARSER_NODE_TYPE_PFX(VAR):
        case  PARSER_NODE_TYPE_PFX(INT):
        case  PARSER_NODE_TYPE_PFX(STRING):
            parser_node_buf_free(node->data.buf);
            break;
        case  PARSER_NODE_TYPE_PFX(FN):
            break;
        case  PARSER_NODE_TYPE_PFX(ASSIGN):
        case  PARSER_NODE_TYPE_PFX(DEFINE):
        case  PARSER_NODE_TYPE_PFX(ADD):
        case  PARSER_NODE_TYPE_PFX(SUB):
        case  PARSER_NODE_TYPE_PFX(MUL):
        case  PARSER_NODE_TYPE_PFX(DIV):
            parser_node_op_free(node->data.op);
            break;
    }
    free(node);
}

extern inline void parser_state_init(parser_state *const ps, const char *const str);

static parser_status parser_token_get(parser_state *const ps, token *const t, bool ignore_nl, bool advance) {
    for (;;) {
        if ((ps->t_status = token_get(&ps->t_state, t, ps->str, advance)) != TOKEN_STATUS_PFX(OK))
            return PARSER_STATUS_PFX(TOKENIZER_ERROR);
        if (t->type != TOKEN_PFX(COMMENT) && t->type != TOKEN_PFX(NEWLINE)) break;
        else if (t->type == TOKEN_PFX(NEWLINE) && ignore_nl == false) break;
    }
    return PARSER_STATUS_PFX(OK);
}

static parser_status parser_token_next(parser_state *const ps, bool ignore_nl) {
    return parser_token_get(ps, &ps->tn, ignore_nl, true);
}

#define TOKEN_NEXT(IGNORE_NL) \
    if ((status = parser_token_next(ps, IGNORE_NL)) != PARSER_STATUS_PFX(OK)) return status

static parser_status parser_token_peek(parser_state *const ps, bool ignore_nl) {
    return parser_token_get(ps, &ps->tp, ignore_nl, false);
}

#define TOKEN_PEEK(IGNORE_NL) \
    if ((status = parser_token_peek(ps, IGNORE_NL)) != PARSER_STATUS_PFX(OK)) return status

parser_status parser_parse_exp(parser_state *const ps, parser_node **node) {
    parser_status status;
    parser_node *tmp_node = NULL;
    TOKEN_NEXT(false);
    switch (ps->tn.type) {
        case TOKEN_PFX(NOTHING):
            return PARSER_STATUS_PFX(NO_TOKEN_FOUND);
        case TOKEN_PFX(VAR):
            TOKEN_PEEK(false);
            if (ps->tp.type == TOKEN_PFX(DEFINE)) {
                tmp_node = parser_node_init(PARSER_NODE_TYPE_PFX(DEFINE), (parser_node_data) { .op = parser_node_op_init() });
                tmp_node->data.op->left = parser_node_init(PARSER_NODE_TYPE_PFX(VAR), (parser_node_data) { .buf = parser_node_buf_init(&ps->tn, ps->str) });
                TOKEN_NEXT(false); // at define
                TOKEN_NEXT(false); // at type
                // TODO type
            }
            TOKEN_TO_BUF(VAR);
        TOKEN_TO_BUF_CASE(INT);
        case TOKEN_PFX(END):
        case TOKEN_PFX(NEWLINE):
        case TOKEN_PFX(SEMICOLON):
            break;
        TOKEN_TO_OP(ASSIGN);
        TOKEN_TO_OP(ADD);
        TOKEN_TO_OP(SUB);
        TOKEN_TO_OP(MUL);
        TOKEN_TO_OP(DIV);
    }
    return PARSER_STATUS_PFX(OK);
}
