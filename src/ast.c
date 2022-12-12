
#include "ast.h"

extern inline ast_node_list *ast_node_list_init(size_t len);

extern inline ast_node_op *ast_node_op_init(void);

extern inline ast_status ast_state_init(ast_state *const as, parser_parse *const parser_fn, char *const str);
