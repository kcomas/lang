
#pragma once

#include <stdint.h>
#include "type.h"
#include "parser.h"

#define AST_TYPE_PFX(NAME) AST_TYPE_PFX_##NAME

typedef enum {
    AST_TYPE_PFX(VAR),
    AST_TYPE_PFX(INT),
    AST_TYPE_PFX(STR),
    AST_TYPE_PFX(LIST),
    AST_TYPE_PFX(ADD),
    AST_TYPE_PFX(SUB),
    AST_TYPE_PFX(MUL),
    AST_TYPE_PFX(DIV)
} ast_type;

typedef struct _ast_node ast_node;

typedef struct {
    size_t len;
    ast_node *nodes[];
} ast_node_list;

inline ast_node_list *ast_node_list_init(size_t len) {
    ast_node_list *nl = calloc(1, sizeof(ast_node_list) + sizeof(ast_node*) * len);
    nl->len = len;
    return nl;
}

typedef struct {
    ast_node *left, *right; // t_type in the ast_node is the ops return type
} ast_node_op;

inline ast_node_op *ast_node_op_init(void) {
    return calloc(1, sizeof(ast_node_op));
}

typedef union {
    type_sym_tbl_item *var;
    ast_node_list *list;
    ast_node_op *op;
    union {
        uint64_t u64;
        int64_t i64;
    } a_int;
} ast_data;

typedef struct _ast_node {
    ast_type a_type;
    type *t_type;
    ast_data data;
} ast_node;

inline ast_node *ast_node_init(ast_type a_type, type *t_type, ast_data data) {
    ast_node *an = calloc(1, sizeof(ast_node));
    an->a_type = a_type;
    an->t_type = t_type;
    an->data = data;
    return an;
}

#define AST_STATUS_PFX(NAME) AST_TYPE_PFX_##NAME

typedef enum {
    AST_STATUS_PFX(OK),
    AST_STATUS_PFX(PARSER_FAIL)
} ast_status;

typedef struct {
    parser_status p_status;
    parser_state p_state;
    parser_node *p_node;
} ast_state;

inline ast_status ast_state_init(ast_state *const as, parser_parse *const parser_fn, char *const str) {
    parser_state_init(&as->p_state, str);
    if ((as->p_status = parser_fn(&as->p_state, &as->p_node)) != PARSER_STATUS_PFX(OK)) return AST_STATUS_PFX(PARSER_FAIL);
    return AST_STATUS_PFX(OK);
}

ast_status ast_expr(type *const fn_mod, parser_node *const p_node, ast_node **a_node, type **parent);
