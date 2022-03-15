
#pragma once

#define AST_TYPE_PFX(NAME) AST_TYPE_PFX_##NAME

typedef enum {
    AST_TYPE_PFX(VAR),
    AST_TYPE_PFX(INT),
    AST_TYPE_PFX(STR)
} ast_type;
