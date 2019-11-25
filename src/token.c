#include <limits.h>
#include <sys/user.h>         // MAX_ARG_STRLEN
#include <linux/binfmts.h>    // MAX_ARG_STRLEN
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "token.h"


bool streq(const char *str1, const char *str2) {
    if (strlen(str1) != strlen(str2))
        return false;
    return (bool) (memcmp(str1, str2, strlen(str1)) == 0);
}


TokenType gettokentype(const char *token) {
    if (streq("IF", token)) {
        return IF_EQ;
    } else if (streq("NIF", token)) {
        return IF_NE;
    }
    return STR;
}


Token *gettoken(FILE *stream, Token *tok) {
    char c;
    bool in_quotes = false;
    bool in_dquotes = false;

    int tmpi = 0;
    char tmpl[MAX_ARG_STRLEN] = {'\0'};

    Token *token;
    token = malloc(sizeof(Token));
    token->index = tok->index + 1;
    token->type = UNKNOWN;
    token->data = NULL;
    token->next = NULL;
    token->prev = tok;

    tok->next = token;

    while ((c = getc(stream)) != EOF) {
        switch (c) {
            case '\n':
            case ' ':
                if (tmpi == 0) {
                    continue;
                }
                if (!in_quotes && !in_dquotes){
                    tmpl[tmpi++] = '\0';
                    token->type = gettokentype(tmpl);
                    token->data = malloc(tmpi * sizeof(char));
                    strncpy(token->data, tmpl, tmpi);

                    if (c == '\n') {
                        Token *t;
                        t = malloc(sizeof(Token));
                        t->index = token->index + 1;
                        t->type = NEWLINE;
                        t->data = NULL;
                        t->next = NULL;
                        t->prev = token;
                        token->next = t;
                        return t;
                    }
                    return token;
                } else {
                    tmpl[tmpi++] = c;
                }
                break;
            case '\'':
                if (in_dquotes){
                    tmpl[tmpi++] = c;
                    break;
                }
                in_quotes = !in_quotes;
                break;
            case '"':
                if (in_quotes){
                    tmpl[tmpi++] = c;
                    break;
                }
                in_dquotes = !in_dquotes;
                break;
            case '\t':
                // Only throw out an INDENT if it's got a newline before it
                if (token->prev->type == NEWLINE) {
                    token->type = INDENT;
                    return token;
                }
            default:
                tmpl[tmpi++] = c;
        }
    }
    return NULL;
}


Token *token_find_nextof(Token *head, TokenType type) {
    Token *t = head;
    while (t != NULL) {
        if (t->type == type)
            return t;
        t = t->next;
    }
    return NULL;
}

Token *token_find_last_conditional(Token *head) {
    Token *t = head;
    while (t != NULL) {
        if (t->type == IF_EQ || t->type == IF_NE)
            return t;
        t = t->prev;
    }
    return NULL;
}

void token_follow_free(Token *temp) {
    if(temp == NULL)
        return;
    if (temp->data)
        free(temp->data);
    if (temp->next != NULL)
        token_follow_free(temp->next);
    free(temp);
}
