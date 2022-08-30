
#include "parser.h"

extern inline bool parser_node_type_is_buf(parser_node_type type);

extern inline bool parser_node_type_is_type(parser_node_type type);

extern inline bool parser_node_type_is_list(parser_node_type type);

extern inline bool parser_node_type_is_get(parser_node_type type);

extern inline bool parser_node_type_is_op(parser_node_type type);

extern inline parser_node_list *parser_node_list_init(void);

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

extern inline parser_node_get *parser_node_get_init(parser_node *tgt);

extern inline void parser_node_get_free(parser_node_get *get);

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
        case PARSER_NODE_TYPE_PFX(STR):
            parser_node_buf_free(node->data.buf);
            break;
        case PARSER_NODE_TYPE_PFX(U64):
            break;
        case PARSER_NODE_TYPE_PFX(MOD):
        case PARSER_NODE_TYPE_PFX(VEC):
            parser_node_list_free(node->data.list);
            free(node->data.list);
            break;
        case PARSER_NODE_TYPE_PFX(FN):
            parser_node_fn_free(node->data.fn);
            break;
        case PARSER_NODE_TYPE_PFX(INDEX):
        case PARSER_NODE_TYPE_PFX(CALL):
            parser_node_get_free(node->data.get);
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

extern inline void parser_state_init(parser_state *const ps, char *const str);

static parser_status parser_token_get(parser_state *const ps, token *const t, bool ignore_nl, bool advance) {
    for (;;) {
        if ((ps->t_status = token_get(&ps->t_state, t, ps->str, advance)) != TOKEN_STATUS_PFX(OK))
            return PARSER_STATUS_PFX(TOKENIZER_ERROR);
        if (t->type != TOKEN_PFX(COMMENT) && t->type != TOKEN_PFX(NEWLINE)) break;
        else if (t->type == TOKEN_PFX(NEWLINE) && !ignore_nl) break;
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

typedef bool stmt_token_type_stop_fn(token_type type);

static parser_status parser_parse_stmt(parser_state *const ps, parser_node_list *const nl, stmt_token_type_stop_fn *const stop_fn) {
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
    return type == TOKEN_PFX(RPARENS) || type == TOKEN_PFX(NEWLINE); // stop \n separation and error
}

bool bracket_token_type_stop(token_type type) {
    return type == TOKEN_PFX(RBRACKET) || type == TOKEN_PFX(NEWLINE); // stop \n separation and error
}

bool fn_body_token_type_stop(token_type type) {
    return type == TOKEN_PFX(RBRACE);
}

bool mod_token_type_stop(token_type type) {
    return type == TOKEN_PFX(END);
}

#define STMT_TO_STOP(NODE, LIST, STOP_FN, STOP_TOKEN, NODE_FREE_FN, ERROR) do { \
        if ((status = parser_parse_stmt(ps, LIST, &STOP_FN)) != PARSER_STATUS_PFX(OK)) return status; \
        /* make sure STOP_TOKEN was reached */ \
        if (ps->tn.type != TOKEN_PFX(STOP_TOKEN)) { \
            NODE_FREE_FN(NODE); \
            return PARSER_STATUS_PFX(ERROR); \
        } \
    } while(0) \

static parser_status parser_parse_fn(parser_state *const ps, parser_node **node) {
    parser_status status;
    if (*node != NULL) return PARSER_STATUS_PFX(NODE_FOR_MOD_FN_NOT_NULL);
    // start at first {
    token t_tmp = ps->tn;
    TOKEN_NEXT(false); // at (
    parser_node_fn *fn = parser_node_fn_init();
    STMT_TO_STOP(fn, &fn->args, parens_token_type_stop, RPARENS, parser_node_fn_free, INVALID_FN_ARGS);
    if (fn->args.len > MAX_ARGS + 1) return PARSER_STATUS_PFX(TOO_MANY_FN_ARGS); // more then ret + max args
    STMT_TO_STOP(fn, &fn->body, fn_body_token_type_stop, RBRACE, parser_node_fn_free, INVALID_FN_BODY);
    *node = parser_node_init(PARSER_NODE_TYPE_PFX(FN), &t_tmp, (parser_node_data) { .fn = fn });
    return parser_parse_expr(ps, node);
}

static parser_status parser_parser_vec_index(parser_state *const ps, parser_node **node) {
    parser_status status;
    // start the list at first [
    token t_tmp = ps->tn;
    // if the previous node is null or its a op with no args [] is a vec, otherwise [] is an index
    if (*node == NULL || (parser_node_type_is_op((*node)->type) && parser_node_op_empty((*node)->data.op))) {
        parser_node_list *vec = parser_node_list_init();
        STMT_TO_STOP(vec, vec, bracket_token_type_stop, RBRACKET, parser_node_list_free, INVALID_VEC);
        parser_node *tmp_node  = parser_node_init(PARSER_NODE_TYPE_PFX(VEC), &t_tmp, (parser_node_data) { .list = vec });
        if (*node == NULL) {
            *node = tmp_node;
        } else {
            // monadic op on vec
            (*node)->data.op->right = tmp_node;
            node = &(*node)->data.op->right;
        }
    } else {
        parser_node_get *get = parser_node_get_init(*node);
        STMT_TO_STOP(get, &get->args, bracket_token_type_stop, RBRACKET, parser_node_get_free, INVALID_INDEX);
        *node = parser_node_init(PARSER_NODE_TYPE_PFX(INDEX), &t_tmp, (parser_node_data) { .get = get });
    }
    return parser_parse_expr(ps, node);
}

static parser_status parser_parse_call(parser_state *const ps, parser_node **node) {
    parser_status status;
    if (*node == NULL) return PARSER_STATUS_PFX(NODE_FOR_CALL_NULL);
    // start the list at the first (
    token t_tmp = ps->tn;
    parser_node_get *get = parser_node_get_init(*node);
    STMT_TO_STOP(get, &get->args, parens_token_type_stop, RPARENS, parser_node_get_free, INVALID_CALL);
    if (get->args.len > MAX_ARGS) return PARSER_STATUS_PFX(TOO_MANY_CALL_ARGS);
    *node = parser_node_init(PARSER_NODE_TYPE_PFX(CALL), &t_tmp, (parser_node_data) { .get = get });
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
        case TOKEN_PFX(LBRACKET):
            return parser_parser_vec_index(ps, node);
        case TOKEN_PFX(LPARENS):
            return parser_parse_call(ps, node);
        case TOKEN_PFX(RBRACE):
        case TOKEN_PFX(RBRACKET):
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

parser_status parser_parser_mod(parser_state *const ps, parser_node **node) {
parser_status status;
    if (*node != NULL) return PARSER_STATUS_PFX(NODE_FOR_MOD_FN_NOT_NULL);
    // use the starting of the file with 0 line and 0 char
    token t_tmp = { 0 };
    parser_node_list *mod = parser_node_list_init();
    STMT_TO_STOP(mod, mod, mod_token_type_stop, END, parser_node_list_free, INVALID_MODULE);
    *node = parser_node_init(PARSER_NODE_TYPE_PFX(MOD), &t_tmp, (parser_node_data) { .list = mod });
    return PARSER_STATUS_PFX(OK);
}
