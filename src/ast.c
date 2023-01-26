
#include "ast.h"

extern inline ast_node_list *ast_node_list_init(size_t len);

extern inline ast_node_op *ast_node_op_init(void);

extern inline ast_node *ast_node_init(ast_type a_type, ast_data data);

extern inline ast_status ast_state_init(ast_state *const as, parser_parse *const parser_fn, char *const str);

ast_status ast_expr(type *const fn_mod, parser_node *const p_node, ast_node **a_node, type **parent) {
    ast_node *tmp_a_node = NULL;
    type *child = NULL;
    type_sym_tbl_item *tmp_var = NULL;
    switch (p_node->type) {
        case PARSER_NODE_TYPE_PFX(U64):
            // TODO
            break;
        case PARSER_NODE_TYPE_PFX(VAR):
            // check if exists in sym_tbl or add it and take the parents type
            // create new node
            break;
    }
    return AST_STATUS_PFX(OK);
}
