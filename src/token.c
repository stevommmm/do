#include <sys/user.h>         // PAGE_SIZE for MAX_ARG_STRLEN
#include <linux/binfmts.h>    // MAX_ARG_STRLEN
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "token.h"

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

/** Explicit wrapper to memcmp that checks equal length
 *
 *  \param[in]  str1  string to compare
 *  \param[in]  str2  string to compare
 *
 *  \return  true if strings are same length and content, otherwise false
 */
bool streq(const char *str1, const char *str2) {
    if (strlen(str1) != strlen(str2))
        return false;
    return (bool) (memcmp(str1, str2, strlen(str1)) == 0);
}

/** Check if our string matches %variable_name%
 *
 *  \param[in]  str  string to compare
 *
 *  \return  true if the start/ending characters are a '%', false otherwise
 */
bool strisvar(const char *str) {
    return (bool) (str[0] == '%' && str[strlen(str) - 1] == '%');
}

/** Check a string against known keywords
 *
 *  \param[in]  token  string to compare (IF|NOT|SYNC|DO)
 *
 *  \return  type of token found, defaults to STR unless explicit match
 */
TokenType gettokentype(const char *token) {
    if (streq("IF", token)) {
        return IF;
    } else if (streq("NOT", token)) {
        return NOT;
    } else if (streq("SYNC", token)) {
        return SYNC;
    } else if (streq("DO", token)) {
        return DO;
    } else if (streq("SET", token)) {
        return SET;
    } else if (strisvar(token)) {
        return VAR;
    }
    return STR;
}

/** Process the FILE stream into a doubly linked list of Tokens
 *
 *  \param[in]  stream  pointer file we're working with, is mutated
 *  \param[in]  tok     Token to append the next parsed element to
 *
 *  \return  pointer to malloc'd Token, NULL we're done parsing
 */
Token *gettoken(FILE *stream, Token *tok) {
    int i;
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
    token->indent = tok->indent;

    tok->next = token;

    while ((i = getc(stream)) != EOF) {
        unsigned char c = i;
        switch (c) {
            case '\n':
            case ' ':
                if (c == ' ' && tmpi == 0) {
                    break;
                }
                if (!in_quotes && !in_dquotes){
                    tmpl[tmpi++] = '\0';
                    token->type = gettokentype(tmpl);
                    token->data = malloc(tmpi * sizeof(unsigned char));
                    strncpy(token->data, tmpl, tmpi);

                    // When we find a newline, dump whatever was in the string
                    // and create a new token node of NEWLINE type after it
                    if (c == '\n') {
                        Token *t;
                        t = malloc(sizeof(Token));
                        t->index = token->index + 1;
                        t->type = NEWLINE;
                        t->data = NULL;
                        t->next = NULL;
                        t->prev = token;
                        token->next = t;
                        t->indent = 0;
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
                if (token->prev->type == NEWLINE || token->prev->type == INDENT) {
                    token->type = INDENT;
                    token->indent++;
                    return token;
                }
            default:
                tmpl[tmpi++] = c;
        }
    }
    return NULL;
}

/** Follow the list forwards until TokenType specified is found
 *
 *  \param[in]  head  pointer to starting location in the list
 *  \param[in]  type  TokenType we're searching for
 *
 *  \return  pointer to matching Token, NULL if not found
 */
Token *token_find_next_of(Token *head, TokenType type) {
    Token *t = head;
    while (t != NULL) {
        if (t->type == type)
            return t;
        t = t->next;
    }
    return NULL;
}

/** Follow the list backwards until we hit an IF token type
 *
 *  \param[in]  head  pointer to starting location in the list
 *
 *  \return  pointer to Token conditional, NULL if not found
 */
Token *token_find_last_conditional(Token *head, int indent_level) {
    Token *t = head;
    while (t != NULL) {
        if (t->type == IF) {
            // If we find a conditional at a lower indent level, we've gone too far
            if (t->indent < indent_level) {
                return NULL;
            }
            return t;
        }
        t = t->prev;
    }
    return NULL;
}

/** Replace all VARs we get from head onwards that match
 *
 *  \param[in]  head  pointer to the VAR we're setting with the content of:
 *  \param[in]  var   pointer to replacement content
 */
void token_sub_var(Token *head, Token *var) {
    Token *t = var;
    int vari = strlen(var->data);
    size_t vart = (vari + 1) * sizeof(unsigned char);

    while (t != NULL) {
        if (t->type == VAR && streq(t->data, head->data)) {
            // swap out our VAR for a STR (probably)
            t->type = var->type;
            t->data = realloc(t->data, vart);
            strncpy(t->data, var->data, vart);
        }
        t = t->next;
    }
}

/** Recursively free elements from the list
 *
 *  \param[in] temp  pointer to a list member, preferrably the head
 */
void token_follow_free(Token *temp) {
    Token *t;
    while (temp != NULL) {
        t = temp;
        temp = temp->next;
        if (t->data)
            free(t->data);
        free(t);
    }
}


/** Pretty print the token
 *
 *  \param[in] token
 */
void token_print(Token *token) {
    int i;
    printf("%d", token->index);
    for (i = 0; i <= token->indent; i++)
        printf("  ");
    switch (token->type) {
        case BEGINNING:
            printf("BEGINNING ");
            break;
        case IF:
            printf("IF ");
            break;
        case NOT:
            printf("NOT ");
            break;
        case STR:
            printf("STR '%s' ", token->data);
            break;
        case NEWLINE:
            printf("NEWLINE ");
            break;
        case INDENT:
            printf("INDENT ");
            break;
        case SYNC:
            printf("SYNC ");
            break;
        case DO:
            printf("DO ");
            break;
        case SET:
            printf("SET ");
            break;
        case VAR:
            printf("VAR '%s' ", token->data);
            break;
        case UNKNOWN:
            printf("UNKNOWN ");
            break;
    }
    printf("\n");
}
