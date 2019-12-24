// vim: set syntax=c et ts=4:
#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include <stdio.h>

typedef enum { BEGINNING, IF_EQ, IF_NE, DO, SYNC, STR, NEWLINE, INDENT, UNKNOWN } TokenType;

typedef struct Token {
    int index;
    TokenType type;
    char *data;
    struct Token *next;
    struct Token *prev;
    unsigned int indent;
    bool passed;
} Token;

Token *gettoken(FILE *stream, Token *tok);
Token *token_find_next_of(Token *head, TokenType type);
Token *token_find_last_conditional(Token *head, int indent_level);
void token_follow_free(Token *head);
void token_print(Token *token);

#endif /* TOKEN_H */
