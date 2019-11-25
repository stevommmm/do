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
void l_free(Token *head);

#endif /* TOKEN_H */
