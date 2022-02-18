
#pragma once

#include "token.h"

typedef struct {
    token_status t_status;
    token_state t_state;
    const char *str;
} parser_state;
