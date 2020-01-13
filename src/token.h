// vim: set syntax=c et ts=4:
#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include <stdio.h>

typedef enum { BEGINNING, IF, NOT, DO, SYNC, STR, NEWLINE, INDENT, VAR, SET, FAILURE, UNKNOWN } TokenType;

typedef struct Token {
    int index;
    TokenType type;
    char *data;
    struct Token *next;
    struct Token *prev;
    unsigned int indent;
    union {
        bool passed;  // Conditionals result
        char *value;  // Used in variables to point to value
    };
} Token;

Token *token_alloc();
Token *gettoken(FILE *stream, Token *tok);
Token *token_find_next_of(Token *head, TokenType type);
Token *token_find_last_conditional(Token *head, int indent_level);
bool token_last_cond_passed(Token *head);
bool token_begins_line(Token *head);
void token_sub_var(Token *head, Token *var);
void token_follow_free(Token *head);
void token_print(Token *token);

#endif /* TOKEN_H */
