
#include "ast.h"

extern inline ast_node_list *ast_node_list_init(size_t len);

extern inline ast_node_op *ast_node_op_init(void);

extern inline ast_node *ast_node_init(ast_type a_type, parser_node *p_node, ast_data data);

extern inline ast_status ast_state_init(ast_state *const as, parser_parse *const parser_fn, char *const str);

extern inline void ast_state_free(ast_state *const as);

ast_status ast_expr(ast_state *const as, type *const fn_mod, parser_node *const p_node, ast_node **a_node, type *const parent) {
    ast_node *tmp_a_node = NULL;
    type *child = NULL;
    type_sym_tbl_item *tmp_var = NULL;
    type_sym_tbl *tbl = type_get_sym_tbl(fn_mod);
    if (tbl == NULL) return AST_STATUS_PFX(SYM_TBL_NOT_FOUND);
    switch (p_node->type) {
        case PARSER_NODE_TYPE_PFX(U64):
            // TODO
            break;
        case PARSER_NODE_TYPE_PFX(VAR):
            // check if exists in sym_tbl or add it and take the parents type
            as->tbl_status = type_sym_tbl_findsert(&tbl, &tmp_var, p_node->data.buf->len, p_node->data.buf->buf, parent);
            if (!type_sym_tble_status_ok(as->tbl_status)) return AST_STATUS_PFX(SYM_TBL_ERROR);
            // create new node
            break;
    }
    return AST_STATUS_PFX(OK);
}
