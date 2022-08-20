
#pragma once

#include <stdint.h>
#include "type.h"
#include "parser.h"

#define AST_TYPE_PFX(NAME) AST_TYPE_PFX_##NAME

typedef enum {
    AST_TYPE_PFX(VAR),
    AST_TYPE_PFX(INT),
    AST_TYPE_PFX(STR),
    AST_TYPE_PFX(FN)
} ast_type;

typedef struct {
    type *t_int;
    union {
        uint64_t u64;
        int64_t i64;
    } value;
} ast_int;

typedef struct _ast_node ast_node;

typedef struct {
    size_t body_len;
    type *t_fn;
    ast_node *body[];
} ast_fn;

typedef union {
    type_sym_tbl_item *var;
    ast_int *aint;
    ast_fn *fn;
} ast_data;

typedef struct _ast_node {
    ast_type type;
    ast_data data;
} ast_node;

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

inline ast_status ast_state_init(ast_state *const as, char *const str) {
    parser_state_init(&as->p_state, str);
    as->p_node = NULL;
    p_status = parser_parse_module(&as->p_state, &as->p_module);
    if (p_status != PARSER_STATUS_PFX(OK)) return AST_STATUS_PFX(PARSER_FAIL);
    return AST_STATUS_PFX(OK);
}
