
#pragma once

#include <string.h>
#include "token.h"

#define PARSER_NODE_TYPE_PFX(NAME) PARSER_NODE_TYPE_PFX_##NAME

typedef enum {
    PARSER_NODE_TYPE_PFX(VAR),
    PARSER_NODE_TYPE_PFX(INT),
    PARSER_NODE_TYPE_PFX(FLOAT),
    PARSER_NODE_TYPE_PFX(STRING),
    PARSER_NODE_TYPE_PFX(FN),
    PARSER_NODE_TYPE_PFX(ASSIGN),
    PARSER_NODE_TYPE_PFX(ADD),
    PARSER_NODE_TYPE_PFX(SUB),
    PARSER_NODE_TYPE_PFX(MUL),
    PARSER_NODE_TYPE_PFX(DIV),
} parser_node_type;

typedef struct _parser_node parser_node;

typedef struct _parser_node_list_item {
    const parser_node *node;
    struct _parser_node_list_item *next;
} parser_node_list_item;

typedef struct _parser_node_list {
    parser_node_list_item *head, *tail;
} parser_node_list;

inline void parser_node_list_add_item(parser_node_list *const nl, const parser_node *const node) {
    parser_node_list_item *item = calloc(1, sizeof(parser_node_list_item));
    item->node = node;
    if (nl->head == NULL) {
        nl->head = item;
        nl->tail = nl->head;
    } else {
        nl->tail->next = item;
        nl->tail = nl->tail->next;
    }
}

typedef struct {
    size_t size; // size does not include null term
    char buf[]; // must be null termed
} parser_node_buf;

inline parser_node_buf *parser_node_buf_init(const token *const t, const char *const str) {
    size_t size = t->end_pos - t->start_pos + 1; // conform to token length
    parser_node_buf *b = calloc(1, sizeof(parser_node_buf) + sizeof(char) * size + sizeof(char)); // null byte
    b->size = size;
    memcpy(b->buf, str + t->start_pos, b->size);
    return b;
}

typedef struct {
    parser_node *left, *right;
} parser_node_op;

inline parser_node_op *parser_node_op_init(void) {
    return calloc(1, sizeof(parser_node_op));
}

#define MAX_ARGS 8

typedef struct {
    size_t num_args;
    parser_node *return_type;
    parser_node_list args;
    parser_node_list body;
} parser_node_fn;

inline parser_node_fn *parser_node_fn_init(void) {
    return calloc(1, sizeof(parser_node_fn));
}

typedef union {
    parser_node_buf *buf;
    parser_node_op *op;
    parser_node_fn *fn;
} parser_node_data;

typedef struct _parser_node {
    parser_node_type type;
    parser_node_data data;
} parser_node; // one created never mutate

inline parser_node *parser_node_init(parser_node_type type, parser_node_data data) {
    parser_node *node = calloc(1, sizeof(parser_node));
    node->type = type;
    node->data = data;
    return node;
}

inline bool parser_node_is_buf(const parser_node *const node) {
    return node->type >= PARSER_NODE_TYPE_PFX(VAR) && node->type <= PARSER_NODE_TYPE_PFX(STRING);
}

inline bool parser_node_is_op(const parser_node *const node) {
    return node->type >= PARSER_NODE_TYPE_PFX(ASSIGN) && node->type <= PARSER_NODE_TYPE_PFX(DIV);
}

#define PARSER_STATUS_PFX(NAME) PARSER_STATUS_PFX_##NAME

typedef enum {
    PARSER_STATUS_PFX(OK),
    PARSER_STATUS_PFX(NO_TOKEN_FOUND),
    PARSER_STATUS_PFX(TOKENIZER_ERROR)
} parser_status;

typedef struct {
    token_status t_status;
    token_state t_state;
    token *tn, *tp;
    const char *str;
} parser_state;

inline void parser_state_init(parser_state *const ps, const char *const str) {
    token_state_init(&ps->t_state);
    ps->tn = calloc(1, sizeof(token));
    token_init(ps->tn);
    ps->tp = calloc(1, sizeof(token));
    token_init(ps->tp);
    ps->str = str;
}

// parent fn holds found node and initialized to NULL
parser_status parser_parse_exp(parser_state *const ps, parser_node **node);
