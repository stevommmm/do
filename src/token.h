#ifndef TOKEN_H
#define TOKEN_H

typedef enum { BEGINNING, IF_EQ, IF_NE, COMMAND } TokenType;

typedef struct Token {
    TokenType type;
    char *data;
    struct Token *next;
    struct Token *prev;
} Token;

void l_free(Token *head);

#endif /* TOKEN_H */
