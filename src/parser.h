
#pragma once

#include "token.h"

#define PARSER_NODE_TYPE_PFX(NAME) PARSER_NODE_TYPE_PFX_##NAME

typedef enum {
    PARSER_NODE_TYPE_PFX(VAR),
    PARSER_NODE_TYPE_PFX(INT),
    PARSER_NODE_TYPE_PFX(FLOAT),
    PARSER_NODE_TYPE_PFX(STRING),
    PARSER_NODE_TYPE_PFX(FN),
    PARSER_NODE_TYPE_PFX(ASSIGN)
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
    size_t len; // len does not include null term
    char buf[]; // must be null termed
} parser_node_buf;

#define MAX_ARGS 8

typedef struct {
    size_t num_args;
    parser_node *return_type;
    parser_node_list args;
    parser_node_list body;
} parser_node_fn;

typedef union {
    parser_node_buf *buf;
    parser_node_fn *fn;
} parser_node_data;

typedef struct _parser_node {
    const parser_node_type type;
    const parser_node_data data;
} parser_node;

#define PARSER_STATUS_PFX(NAME) PARSER_STATUS_PFX_##NAME

typedef enum {
    PARSER_STATUS_PFX(OK),
    PARSER_STATUS_PFX(TOKENIZER_ERROR)
} parser_status;

typedef struct {
    token_status t_status;
    token_state t_state;
    token *n, *p;
    const char *str;
} parser_state;

inline void parser_state_init(parser_state *const ps, const char *const str) {
    ps->t_status = TOKEN_STATUS_PFX(OK);
    token_state_init(&ps->t_state);
    ps->n = calloc(1, sizeof(token));
    token_init(ps->n);
    ps->p = calloc(1, sizeof(token));
    token_init(ps->p);
    ps->str = str;
}

inline parser_status parser_token_get(parser_state *const ps, token *const t, bool ignore_nl, bool advance) {
    for (;;) {
        if ((ps->t_status = token_get(&ps->t_state, t, ps->str, advance)) != TOKEN_STATUS_PFX(OK))
            return PARSER_STATUS_PFX(TOKENIZER_ERROR);
        if (t->type != TOKEN_PFX(COMMENT) && (ignore_nl == true && t->type != TOKEN_PFX(NEWLINE))) break;
    }
    return PARSER_STATUS_PFX(OK);
}

inline parser_status parser_token_next(parser_state *const ps, bool ignore_nl) {
    return parser_token_get(ps, ps->n, ignore_nl, true);
}

inline parser_status parser_token_peek(parser_state *const ps, bool ignore_nl) {
    return parser_token_get(ps, ps->p, ignore_nl, false);
}
