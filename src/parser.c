
#include "parser.h"

extern inline void parser_node_list_add_item(parser_node_list *const nl, const parser_node *const node);

extern inline parser_node_buf *parser_node_buf_init(const token *const t, const char *const str);

extern inline parser_node_op *parser_node_op_init(void);

#define TOKEN_TO_OP(TYPE) case TOKEN_PFX(TYPE): \
    tmp_node = parser_node_init(PARSER_NODE_TYPE_PFX(TYPE), (parser_node_data) { .op = parser_node_op_init() }); \
    tmp_node->data.op->left = *node; \
    *node = tmp_node; \
    return parser_parse_exp(ps, &tmp_node->data.op->right)

extern inline parser_node_fn *parser_node_fn_init(void);

extern inline parser_node *parser_node_init(parser_node_type type, parser_node_data data);

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
    return parser_token_get(ps, ps->tn, ignore_nl, true);
}

#define TOKEN_NEXT(IGNORE_NL) \
    if ((status = parser_token_next(ps, IGNORE_NL)) != PARSER_STATUS_PFX(OK)) return status

static parser_status parser_token_peek(parser_state *const ps, bool ignore_nl) {
    return parser_token_get(ps, ps->tp, ignore_nl, false);
}

#define TOKEN_TO_BUF(TYPE) case TOKEN_PFX(TYPE): \
    *node = parser_node_init(PARSER_NODE_TYPE_PFX(TYPE), (parser_node_data) { .buf = parser_node_buf_init(ps->tn, ps->str) }); \
    return parser_parse_exp(ps, node)

parser_status parser_parse_exp(parser_state *const ps, parser_node **node) {
    parser_status status;
    parser_node *tmp_node;
    TOKEN_NEXT(false);
    switch (ps->tn->type) {
        case TOKEN_PFX(NOTHING):
            return PARSER_STATUS_PFX(NO_TOKEN_FOUND);
        case TOKEN_PFX(END):
        case TOKEN_PFX(NEWLINE):
            break;
        TOKEN_TO_BUF(VAR);
        TOKEN_TO_BUF(INT);
        TOKEN_TO_OP(ASSIGN);
        TOKEN_TO_OP(ADD);
        TOKEN_TO_OP(SUB);
        TOKEN_TO_OP(MUL);
        TOKEN_TO_OP(DIV);
    }
    return PARSER_STATUS_PFX(OK);
}
