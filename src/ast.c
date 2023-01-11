
#include "ast.h"

extern inline ast_node_list *ast_node_list_init(size_t len);

extern inline ast_node_op *ast_node_op_init(void);

extern inline ast_node *ast_node_init(ast_type a_type, type *t_type, ast_data data);

extern inline ast_status ast_state_init(ast_state *const as, parser_parse *const parser_fn, char *const str);

ast_status ast_expr(type *const fn_mod, parser_node *const p_node, ast_node **a_node, type **parent) {
    ast_node *tmp = NULL;
    type *child = NULL;
    switch (p_node->type) {
        case PARSER_NODE_TYPE_PFX(VAR):
            // check if exists in sym_tbl
            break;
    }
    return AST_STATUS_PFX(OK);
}
