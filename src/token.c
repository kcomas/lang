
#include "token.h"

extern inline token_state *token_state_init(void);

extern inline void token_state_free(token_state *ts);

token_status token_get(token_state *ts, token *t, char *str, bool advance) {

}

extern inline token_status token_next(token_state *ts, token *t, char *str);

extern inline token_status token_peek(token_state *ts, token *t, char *str);
