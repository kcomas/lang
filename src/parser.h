
#pragma once

#include "token.h"

typedef struct {
    token_status t_status;
    const char *str;
    token_state *t_state;
} parser_state;
