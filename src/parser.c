
#include "parser.h"

extern inline void parser_node_list_add_item(parser_node_list *const nl, const parser_node *const node);

extern inline parser_node_buf *parser_node_buf_init(size_t size, size_t start_pos, size_t end_pos, const char *const str);

extern inline parser_node_fn *parser_node_fn_init(void);

extern inline const parser_node *parser_node_init(parser_node_type type, parser_node_data data);

extern inline void parser_state_init(parser_state *const ps, const char *const str);

extern inline parser_status parser_token_get(parser_state *const ps, token *const t, bool ignore_nl, bool advance);

extern inline parser_status parser_token_next(parser_state *const ps, bool ignore_nl);

extern inline parser_status parser_token_peek(parser_state *const ps, bool ignore_nl);
