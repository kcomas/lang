
#pragma once

#include <stdint.h>
#include "type.h"

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
    ast_int *aint;
    ast_fn *fn;
} ast_data;

typedef struct _ast_node {
    ast_type type;
    ast_data data;
} ast_node;
