
#include "token.h"

extern inline void token_init(token *const t);

extern inline void token_state_init(token_state *const ts);

extern inline void token_state_copy(token_state *const dest, const token_state *const src);

static void advance_char_pos(token_state *const ts_tmp) {
    ts_tmp->char_no++;
    ts_tmp->pos++;
}

static char char_at(const token_state *const ts_tmp, const char *const str) {
    return str[ts_tmp->pos];
}

static char char_peek(const token_state *const ts_tmp, const char *const str) {
    return str[ts_tmp->pos + 1];
}

static char char_next(token_state *const ts_tmp, const char *const str) {
    advance_char_pos(ts_tmp);
    return char_at(ts_tmp, str);
}

static void finalize_tokenizer_pass(token_state *const ts, token_state *const ts_tmp, token *const t, bool advance) {
    // now at first char of next token or space
    t->end_pos = ts_tmp->pos - 1;
    if (advance) token_state_copy(ts, ts_tmp);
}

static token_status load_word(token_state *const ts_tmp, token *const t, const char *const str) {
    // we are on the fist char of the word
    char c = char_at(ts_tmp, str);
    while (isalpha(c) || isdigit(c)) c = char_next(ts_tmp, str);
    t->type = TOKEN_PFX(VAR);
    size_t start = t->start_pos;
    size_t len = ts_tmp->pos - start;
    // possible type
    if (len == 2) {
        switch (str[start]) {
            case 'u':
                switch (str[start + 1]) {
                    TOKEN_ONE_CHAR('1', U1);
                    TOKEN_ONE_CHAR('8', U8);
                }
                break;
            case 'i':
                switch (str[start + 1]) {
                    TOKEN_ONE_CHAR('8', I8);
                }
                break;
        }
    } else if (len == 3) {
        switch (str[start]) {
            case 'u':
                switch (str[start + 1]) {
                    TOKEN_TYPE_DOUBLE_SWITCH(str, start, 2, '1', '6', U16);
                    TOKEN_TYPE_DOUBLE_SWITCH(str, start, 2, '3', '2', U32);
                    TOKEN_TYPE_DOUBLE_SWITCH(str, start, 2, '6', '4', U64);
                }
                break;
            case 'i':
                switch (str[start + 1]) {
                    TOKEN_TYPE_DOUBLE_SWITCH(str, start, 2, '1', '6', I16);
                    TOKEN_TYPE_DOUBLE_SWITCH(str, start, 2, '3', '2', I32);
                    TOKEN_TYPE_DOUBLE_SWITCH(str, start, 2, '6', '4', I64);
                }
                break;
            case 'f':
                switch (str[start + 1]) {
                    TOKEN_TYPE_DOUBLE_SWITCH(str, start, 2, '3', '2', F32);
                    TOKEN_TYPE_DOUBLE_SWITCH(str, start, 2, '6', '4', F64);
                }
                break;
        }
    }
    return TOKEN_STATUS_PFX(OK);
}

static token_status load_num(token_state *const ts_tmp, token *const t, const char *const str) {
    // at first digit of num
    char c = char_at(ts_tmp, str);
    while (isdigit(c)) c = char_next(ts_tmp, str);
    // TODO check for other chars
    // TODO float + other num types
    t->type = TOKEN_PFX(INT);
    return TOKEN_STATUS_PFX(OK);
}

static token_status load_string(token_state *const ts_tmp, token *const t, const char *const str) {
    // at first "
    char c = char_next(ts_tmp, str);
    // at first char of str or " for empty str
    while (c != '"') {
        c = char_next(ts_tmp, str);
        if (c == '\0') return TOKEN_STATUS_PFX(INVALID_STRING);
    }
    t->type = TOKEN_PFX(STRING);
    return TOKEN_STATUS_PFX(OK);
}

token_status token_get(token_state *const ts, token *const t, const char *const str, bool advance) {
    // do not use token_state passed in for next token
    token_state ts_tmp;
    token_status status;
    token_state_copy(&ts_tmp, ts);
    // on the char of the next token
    char c = char_at(&ts_tmp, str);
    // check if it is a space \s|\t
    while (c == ' ' || c == '\t') c = char_next(&ts_tmp, str);
    // on char of possible valid token
    t->start_pos = ts_tmp.pos;
    t->line_no = ts_tmp.line_no;
    t->char_no = ts_tmp.char_no;
    if (isalpha(c)) {
        if ((status = load_word(&ts_tmp, t, str)) != TOKEN_STATUS_PFX(OK)) return status;
    } else if (isdigit(c)) {
        if ((status = load_num(&ts_tmp, t, str)) != TOKEN_STATUS_PFX(OK)) return status;
    } else {
        switch (c) {
            case '"':
                if ((status = load_string(&ts_tmp, t, str)) != TOKEN_STATUS_PFX(OK)) return status;
                break;
            TOKEN_ONE_CHAR('\0', END);
            case '\n':
                advance_char_pos(&ts_tmp);
                ts_tmp.line_no++;
                ts_tmp.char_no = 1;
                t->type = TOKEN_PFX(NEWLINE);
                finalize_tokenizer_pass(ts, &ts_tmp, t, advance);
                return TOKEN_STATUS_PFX(OK);
            TOKEN_TWO_CHAR(';', ';', SEMICOLON, RETURN);
            TOKEN_ONE_CHAR('{', LBRACE);
            TOKEN_ONE_CHAR('}', RBRACE);
            TOKEN_ONE_CHAR('[', LBRACKET);
            TOKEN_ONE_CHAR(']', RBRACKET);
            TOKEN_ONE_CHAR('(', LPARENS);
            TOKEN_ONE_CHAR(')', RPARENS);
            TOKEN_TWO_CHAR(':', ':', ASSIGN, DEFINE);
            case '?':
                t->type = TOKEN_PFX(QUESTION);
                c = char_peek(&ts_tmp, str);
                switch (c) {
                    case '?':
                        t->type = TOKEN_PFX(IF);
                        advance_char_pos(&ts_tmp);
                        break;
                    case '@':
                        t->type = TOKEN_PFX(WHILE);
                        advance_char_pos(&ts_tmp);
                        break;
                }
                break;
            TOKEN_TWO_CHAR('|', '|', OR, BITOR);
            TOKEN_TWO_CHAR('=', '=', EQ, DEEPEQ);
            TOKEN_ONE_CHAR('$', CAST);
            TOKEN_ONE_CHAR('+', ADD);
            TOKEN_ONE_CHAR('-', SUB);
            TOKEN_ONE_CHAR('*', MUL);
            case '/':
                c = char_peek(&ts_tmp, str);
                if (c == '/') {
                    // comment read until \n or \0
                    c = char_next(&ts_tmp, str); // c is at second /
                    while (c != '\n' && c != '\0') c = char_next(&ts_tmp, str);
                    t->type = TOKEN_PFX(COMMENT);
                    // c is at \n or \0 must reverse
                    ts_tmp.char_no--;
                    ts_tmp.pos--;
                } else {
                    t->type = TOKEN_PFX(DIV);
                }
                break;
            default:
                return TOKEN_STATUS_PFX(INVALID_CHAR);
            case '<':
                t->type = TOKEN_PFX(LESS);
                c = char_peek(&ts_tmp, str);
                switch (c) {
                    case '&':
                        t->type = TOKEN_PFX(READWRITE);
                        advance_char_pos(&ts_tmp);
                        break;
                }
                break;
        }
        advance_char_pos(&ts_tmp);
    }
    finalize_tokenizer_pass(ts, &ts_tmp, t, advance);
    return TOKEN_STATUS_PFX(OK);
}

extern inline token_status token_next(token_state *const ts, token *const t, const char *const str);

extern inline token_status token_peek(token_state *const ts, token *const t, const char *const str);
