#ifndef TOKEN_H
#define TOKEN_H

typedef enum { BEGINNING, IF_EQ, IF_NE, STR, NEWLINE, INDENT, UNKNOWN } TokenType;

typedef struct Token {
    int index;
    TokenType type;
    char *data;
    struct Token *next;
    struct Token *prev;
} Token;

Token *gettoken(FILE *stream, Token *tok);
Token *token_find_nextof(Token *head, TokenType type);
void token_follow_free(Token *head);

#endif /* TOKEN_H */
