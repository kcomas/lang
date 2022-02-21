
#pragma once

#include "token.h"

#define PARSER_NODE_TYPE_PFX(NAME) PARSER_NODE_TYPE_PFX_##NAME

typedef enum {
    PARSER_NODE_TYPE_PFX(VAR),
    PARSER_NODE_TYPE_PFX(INT),
    PARSER_NODE_TYPE_PFX(FLOAT),
    PARSER_NODE_TYPE_PFX(STRING)
} parser_node_type;

typedef struct {
    size_t len; // len does not include null term
    char buf[]; // must be null termed
} parser_node_buf;

typedef union {
    parser_node_buf *buf;
} parser_node_data;

typedef struct _parser_node {
    parser_node_type type;
    parser_node_data data;
} parser_node;

typedef struct {
    token_status t_status;
    token_state t_state;
    const char *str;
} parser_state;
