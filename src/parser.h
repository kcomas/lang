
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

typedef struct _parser_node_list parser_node_list;

typedef struct {
    size_t len; // len does not include null term
    char buf[]; // must be null termed
} parser_node_buf;

#define MAX_ARGS 8

typedef struct {
    size_t num_args;
    parser_node *return_type;
    parser_node_list arg_types;
    parser_node_list body;
} parser_node_fn;

typedef union {
    parser_node_buf *buf;
    parser_node_fn *fn;
} parser_node_data;

typedef struct _parser_node {
    parser_node_type type;
    parser_node_data data;
} parser_node;

typedef struct _parser_node_list_item {
    parser_node *node;
    struct _parser_node_list_item *next;
} parser_node_list_item;

typedef struct _parser_node_list {
    parser_node_list_item *head, *tail;
} parser_node_list;

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
    parser->n = calloc(1, sizeof(token));
    token_init(parser->n);
    parser->p = calloc(1, sizeof(token));
    token_init(parser->p);
    token_state_init(&ps->t_state);
}

parser_status parser_token_get(parser_state *const ps, token *const t, bool ignore_nl, bool advence);

inline parser_status parser_token_next(parser_state *const ps, bool ignore_nl) {
    return parser_token_get(ps, ps->n, ignore_nl, true);
}

inline parser_status parser_token_next(parser_state *const ps, bool ignore_nl) {
    return parser_token_get(ps, ps->p, ignore_nl, false);
}
