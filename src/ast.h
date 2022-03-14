
#pragma once

#define AST_TYPE_PFX(NAME) AST_TYPE_PFX_##NAME

typedef enum {
    AST_TYPE_PFX(U0),
    AST_TYPE_PFX(U1)
} ast_type;
