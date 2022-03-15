
#pragma once

#include <string.h>
#include "token.h"

#define PARSER_NODE_TYPE_PFX(NAME) PARSER_NODE_TYPE_PFX_##NAME

typedef enum {
    PARSER_NODE_TYPE_PFX(VAR),
    PARSER_NODE_TYPE_PFX(INT),
    PARSER_NODE_TYPE_PFX(STR),
    PARSER_NODE_TYPE_PFX(U64),
    PARSER_NODE_TYPE_PFX(FN),
    PARSER_NODE_TYPE_PFX(VEC),
    PARSER_NODE_TYPE_PFX(INDEX),
    PARSER_NODE_TYPE_PFX(CALL),
    PARSER_NODE_TYPE_PFX(ASSIGN),
    PARSER_NODE_TYPE_PFX(DEFINE),
    PARSER_NODE_TYPE_PFX(ADD),
    PARSER_NODE_TYPE_PFX(SUB),
    PARSER_NODE_TYPE_PFX(MUL),
    PARSER_NODE_TYPE_PFX(EXP),
    PARSER_NODE_TYPE_PFX(DIV)
} parser_node_type;

inline bool parser_node_type_is_buf(parser_node_type type) {
    return type >= PARSER_NODE_TYPE_PFX(VAR) && type <= PARSER_NODE_TYPE_PFX(STR);
}

inline bool parser_node_type_is_type(parser_node_type type) {
    return type >= PARSER_NODE_TYPE_PFX(U64) && type <= PARSER_NODE_TYPE_PFX(U64);
}

inline bool parser_node_type_is_get(parser_node_type type) {
    return type >= PARSER_NODE_TYPE_PFX(INDEX) && type <= PARSER_NODE_TYPE_PFX(CALL);
}

inline bool parser_node_type_is_op(parser_node_type type) {
    return type >= PARSER_NODE_TYPE_PFX(ASSIGN) && type <= PARSER_NODE_TYPE_PFX(DIV);
}

typedef struct _parser_node parser_node;

void parser_node_free(parser_node *node);

typedef struct _parser_node_list_item {
    parser_node *node;
    struct _parser_node_list_item *next;
} parser_node_list_item;

typedef struct _parser_node_list {
    size_t len;
    parser_node_list_item *head, *tail;
} parser_node_list;

inline void parser_node_list_free(parser_node_list *list) {
    parser_node_list_item *head = list->head;
    while (head != NULL) {
        parser_node_list_item *tmp = head;
        head = head->next;
        parser_node_free(tmp->node);
        free(tmp);
    }
}

inline void parser_node_list_add_item(parser_node_list *const nl, parser_node *const node) {
    parser_node_list_item *item = calloc(1, sizeof(parser_node_list_item));
    item->node = node;
    if (nl->head == NULL) nl->head = nl->tail = item;
    else nl->tail = nl->tail->next = item;
    nl->len++;
}

typedef struct {
    size_t len; // len does not include null term
    char buf[]; // must be null termed
} parser_node_buf;

inline parser_node_buf *parser_node_buf_init(const token *const t, const char *const str) {
    size_t len = t->end_pos - t->start_pos + 1; // conform to token length
    parser_node_buf *b = calloc(1, sizeof(parser_node_buf) + sizeof(char) * len + sizeof(char)); // null byte
    memcpy(b->buf, str + t->start_pos, len);
    b->len = len;
    return b;
}

inline void parser_node_buf_free(parser_node_buf *buf) {
    free(buf);
}

#ifndef MAX_ARGS
    #define MAX_ARGS 8
#endif

typedef struct {
    parser_node_list args; // total args is list_len -1 if last arg is type if no define op otherwise fn ret u0
    parser_node_list body;
} parser_node_fn;

inline parser_node_fn *parser_node_fn_init(void) {
    return calloc(1, sizeof(parser_node_fn));
}

inline void parser_node_fn_free(parser_node_fn *fn) {
    parser_node_list_free(&fn->args);
    parser_node_list_free(&fn->body);
    free(fn);
}

typedef struct {
    parser_node_list items;
} parser_node_vec;

inline parser_node_vec *parser_node_vec_init(void) {
    return calloc(1, sizeof(parser_node_vec));
}

inline void parser_node_vec_free(parser_node_vec *vec) {
    parser_node_list_free(&vec->items);
    free(vec);
}

typedef struct {
    parser_node *tgt;
    parser_node_list args;
} parser_node_get;

inline parser_node_get *parser_node_get_init(parser_node *tgt) {
    parser_node_get *get = calloc(1, sizeof(parser_node_get));
    get->tgt = tgt;
    return get;
}

inline void parser_node_get_free(parser_node_get *get) {
    parser_node_free(get->tgt);
    parser_node_list_free(&get->args);
    free(get);
}

typedef struct {
    parser_node *left, *right;
} parser_node_op;

inline parser_node_op *parser_node_op_init(void) {
    return calloc(1, sizeof(parser_node_op));
}

inline void parser_node_op_free(parser_node_op *op) {
    parser_node_free(op->left);
    parser_node_free(op->right);
    free(op);
}

inline bool parser_node_op_empty(const parser_node_op *const op) {
    return op->left == NULL && op->right == NULL;
}

typedef union {
    parser_node_buf *buf;
    parser_node_op *op;
    parser_node_fn *fn;
    parser_node_vec *vec;
    parser_node_get *get;
} parser_node_data;

typedef struct _parser_node {
    parser_node_type type;
    size_t line_no, char_no; // used for errors in ast/infer
    parser_node_data data;
} parser_node;

inline parser_node *parser_node_init(parser_node_type type, const token *const t, parser_node_data data ) {
    parser_node *node = calloc(1, sizeof(parser_node));
    node->line_no = t->line_no;
    node->char_no = t->char_no;
    node->type = type;
    node->data = data;
    return node;
}

#define PARSER_STATUS_PFX(NAME) PARSER_STATUS_PFX_##NAME

typedef enum {
    PARSER_STATUS_PFX(OK),
    PARSER_STATUS_PFX(NO_TOKEN_FOUND),
    PARSER_STATUS_PFX(TOKENIZER_ERROR),
    PARSER_STATUS_PFX(NODE_FOR_BUF_NOT_NULL),
    PARSER_STATUS_PFX(NODE_FOR_TYPE_NOT_NULL),
    PARSER_STATUS_PFX(NODE_FOR_FN_NOT_NULL),
    PARSER_STATUS_PFX(NODE_FOR_CALL_NULL),
    PARSER_STATUS_PFX(INVALID_FN_ARGS),
    PARSER_STATUS_PFX(TOO_MANY_FN_ARGS),
    PARSER_STATUS_PFX(INVALID_FN_BODY),
    PARSER_STATUS_PFX(INVALID_VEC),
    PARSER_STATUS_PFX(INVALID_INDEX),
    PARSER_STATUS_PFX(INVALID_CALL),
    PARSER_STATUS_PFX(TOO_MANY_CALL_ARGS),
    PARSER_STATUS_PFX(INVALID_DEFINE)
} parser_status;

typedef struct {
    token_status t_status;
    token_state t_state;
    token tn, tp;
    char *str;
} parser_state;

inline void parser_state_init(parser_state *const ps, char *const str) {
    token_state_init(&ps->t_state);
    token_init(&ps->tn);
    token_init(&ps->tp);
    ps->str = str;
}

// parent fn holds found node and initialized to NULL
parser_status parser_parse_expr(parser_state *const ps, parser_node **node);
