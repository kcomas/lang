
#include "parser.h"

extern inline bool parser_node_type_is_buf(parser_node_type type);

extern inline bool parser_node_type_is_type(parser_node_type type);

extern inline bool parser_node_type_is_op(parser_node_type type);

extern inline void parser_node_list_free(parser_node_list *list);

extern inline void parser_node_list_add_item(parser_node_list *const nl, parser_node *const node);

extern inline parser_node_buf *parser_node_buf_init(const token *const t, const char *const str);

extern inline void parser_node_buf_free(parser_node_buf *buf);

// previous node must be null or monadic op stop to bufs and types from being next to each other
#define TOKEN_PREV_MAYBE_OP(TYPE, INIT, ERROR) case TOKEN_PFX(TYPE): \
    do { \
        if (*node == NULL) { \
            *node = INIT; \
        } else if (parser_node_type_is_op((*node)->type) && parser_node_op_empty((*node)->data.op)) { \
            (*node)->data.op->right = INIT; \
            node = &(*node)->data.op->right; \
        } else { \
            return PARSER_STATUS_PFX(ERROR); \
        } \
        return parser_parse_expr(ps, node); \
    } while(0)


#define TOKEN_TO_BUF(TYPE) \
    TOKEN_PREV_MAYBE_OP(TYPE, parser_node_init(PARSER_NODE_TYPE_PFX(TYPE), &ps->tn, (parser_node_data) { .buf = parser_node_buf_init(&ps->tn, ps->str) }), NODE_FOR_BUF_NOT_NULL)

#define TOKEN_TO_TYPE(TYPE) \
    TOKEN_PREV_MAYBE_OP(TYPE, parser_node_init(PARSER_NODE_TYPE_PFX(TYPE), &ps->tn, (parser_node_data) {}), NODE_FOR_TYPE_NOT_NULL)

extern inline parser_node_fn *parser_node_fn_init(void);

extern inline void parser_node_fn_free(parser_node_fn *fn);

extern inline parser_node_call *parser_node_call_init(parser_node *fn);

extern inline void parser_node_call_free(parser_node_call *call);

extern inline parser_node_op *parser_node_op_init(void);

extern inline void parser_node_op_free(parser_node_op *op);

extern inline bool parser_node_op_empty(const parser_node_op *const op);

// if the left side is null get right side as monadic
#define TOKEN_TO_OP(TYPE) case TOKEN_PFX(TYPE): \
    tmp_node = parser_node_init(PARSER_NODE_TYPE_PFX(TYPE), &ps->tn, (parser_node_data) { .op = parser_node_op_init() }); \
    tmp_node->data.op->left = *node; \
    *node = tmp_node; \
    if (tmp_node->data.op->left == NULL) return parser_parse_expr(ps, node); \
    return parser_parse_expr(ps, &tmp_node->data.op->right)

extern inline parser_node *parser_node_init(parser_node_type type, const token *const t, parser_node_data data);

void parser_node_free(parser_node *node) {
    if (node == NULL) return;
    switch (node->type) {
        case PARSER_NODE_TYPE_PFX(VAR):
        case PARSER_NODE_TYPE_PFX(INT):
        case PARSER_NODE_TYPE_PFX(STRING):
            parser_node_buf_free(node->data.buf);
            break;
        case PARSER_NODE_TYPE_PFX(U64):
            break;
        case PARSER_NODE_TYPE_PFX(FN):
            parser_node_fn_free(node->data.fn);
            break;
        case PARSER_NODE_TYPE_PFX(CALL):
            parser_node_call_free(node->data.call);
            break;
        case PARSER_NODE_TYPE_PFX(ASSIGN):
        case PARSER_NODE_TYPE_PFX(DEFINE):
        case PARSER_NODE_TYPE_PFX(ADD):
        case PARSER_NODE_TYPE_PFX(SUB):
        case PARSER_NODE_TYPE_PFX(MUL):
        case PARSER_NODE_TYPE_PFX(EXP):
        case PARSER_NODE_TYPE_PFX(DIV):
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

typedef bool stmt_token_type_stop(token_type type);

static parser_status parser_parse_stmt(parser_state *const ps, parser_node_list *const nl, stmt_token_type_stop *stop_fn) {
    parser_status status;
    parser_node *tmp_node;
    for (;;) {
        tmp_node = NULL;
        if ((status = parser_parse_expr(ps, &tmp_node)) != PARSER_STATUS_PFX(OK)) return status;
        if (tmp_node == NULL) break;
        parser_node_list_add_item(nl, tmp_node);
        if (stop_fn(ps->tn.type) == true) break;
    }
    return PARSER_STATUS_PFX(OK);
}

bool parens_token_type_stop(token_type type) {
    return type == TOKEN_PFX(RPARENS) || type == TOKEN_PFX(NEWLINE); // stop \n separation
}

bool fn_body_token_type_stop(token_type type) {
    return type == TOKEN_PFX(RBRACE);
}

static parser_status parser_parse_fn(parser_state *const ps, parser_node **node) {
    parser_status status;
    if (*node != NULL) return PARSER_STATUS_PFX(NODE_FOR_FN_NOT_NULL);
    // start at first {
    token t_tmp = ps->tn;
    TOKEN_NEXT(false); // at (
    parser_node_fn *fn = parser_node_fn_init();
    if ((status = parser_parse_stmt(ps, &fn->args, &parens_token_type_stop)) != PARSER_STATUS_PFX(OK)) return status;
    // ensure ) was reached
    if (ps->tn.type != TOKEN_PFX(RPARENS)) {
        parser_node_fn_free(fn);
        return PARSER_STATUS_PFX(INVALID_FN_ARGS);
    }
    if ((status = parser_parse_stmt(ps, &fn->body, &fn_body_token_type_stop)) != PARSER_STATUS_PFX(OK)) return status;
    // ensure } was reached
    if (ps->tn.type != TOKEN_PFX(RBRACE)) {
        parser_node_fn_free(fn);
        return PARSER_STATUS_PFX(INVALID_FN_BODY);
    }
    *node = parser_node_init(PARSER_NODE_TYPE_PFX(FN), &t_tmp, (parser_node_data) { .fn = fn });
    return parser_parse_expr(ps, node);
}

static parser_status parser_parse_call(parser_state *const ps, parser_node **node) {
    parser_status status;
    if (*node == NULL) return PARSER_STATUS_PFX(NODE_FOR_CALL_NOT_NULL);
    // start the list at the first {
    token t_tmp = ps->tn;
    parser_node_call *call = parser_node_call_init(*node);
    if ((status = parser_parse_stmt(ps, &call->args, &parens_token_type_stop)) != PARSER_STATUS_PFX(OK)) return status;
    // make sure ) was reached
    if (ps->tn.type != TOKEN_PFX(RPARENS)) {
        parser_node_call_free(call);
        return PARSER_STATUS_PFX(INVALID_CALL);
    }
    *node = parser_node_init(PARSER_NODE_TYPE_PFX(CALL), &t_tmp, (parser_node_data) { .call = call });
    return parser_parse_expr(ps, node);
}

parser_status parser_parse_expr(parser_state *const ps, parser_node **node) {
    parser_status status;
    parser_node *tmp_node = NULL;
    TOKEN_NEXT(false);
    switch (ps->tn.type) {
        case TOKEN_PFX(NOTHING):
            return PARSER_STATUS_PFX(NO_TOKEN_FOUND);
        TOKEN_TO_BUF(VAR);
        TOKEN_TO_BUF(INT);
        TOKEN_TO_TYPE(U64);
        case TOKEN_PFX(END):
        case TOKEN_PFX(NEWLINE):
        case TOKEN_PFX(SEMICOLON):
            break;
        case TOKEN_PFX(LBRACE):
            TOKEN_PEEK(false);
            if (ps->tp.type == TOKEN_PFX(LPARENS)) return parser_parse_fn(ps, node);
            // TODO dict
            break;
        case TOKEN_PFX(LPARENS):
            return parser_parse_call(ps, node);
        case TOKEN_PFX(RBRACE):
        case TOKEN_PFX(RPARENS):
            break;
        TOKEN_TO_OP(ASSIGN);
        case TOKEN_PFX(DEFINE):
            tmp_node = parser_node_init(PARSER_NODE_TYPE_PFX(DEFINE), &ps->tn, (parser_node_data) { .op = parser_node_op_init() });
            TOKEN_PEEK(false);
            if (ps->tp.type == TOKEN_PFX(VAR)) {
                TOKEN_NEXT(false); // at var
                tmp_node->data.op->right = parser_node_init(PARSER_NODE_TYPE_PFX(VAR), &ps->tn, (parser_node_data) { .buf = parser_node_buf_init(&ps->tn, ps->str) });
            } else {
                parser_node_free(tmp_node);
                return PARSER_STATUS_PFX(INVALID_DEFINE);
            }
            tmp_node->data.op->left = *node;
            *node = tmp_node;
            return parser_parse_expr(ps, node);
        TOKEN_TO_OP(ADD);
        TOKEN_TO_OP(SUB);
        TOKEN_TO_OP(MUL);
        TOKEN_TO_OP(EXP);
        TOKEN_TO_OP(DIV);
    }
    return PARSER_STATUS_PFX(OK);
}
