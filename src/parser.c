
#include "parser.h"

extern inline void parser_state_init(parser_state *const ps, const char *const str);

parser_status parser_token_get(parser_state *const ps, token *const t, bool ignore_nl, bool advence) {
    for (;;) {
        if ((ps->t_status = token_get(ps->t_state, t, ps->str, advance)) != TOKEN_STATUS_PFX(OK))
            return PARSER_STATUS_PFX(TOKENIZER_ERROR);
        if (t->type != TOKEN_PFX(COMMENT) && (ignore_nl == true && t->type != TOKEN_PFX(NEWLINE)) break;
    }
    return PARSER_STATUS_PFX(OK);
}

extern inline parser_status parser_token_next(parser_state *const ps, bool ignore_nl);

extern inline parser_status parser_token_next(parser_state *const ps, bool ignore_nl);
