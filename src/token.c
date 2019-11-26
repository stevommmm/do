#include <sys/user.h>         // MAX_ARG_STRLEN
#include <linux/binfmts.h>    // MAX_ARG_STRLEN
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

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

/** Check a string against known keywords
 *
 *  \param[in]  token  string to compare [N]IF
 *
 *  \return  type of token found, defaults to STR unless explicit match
 */
TokenType gettokentype(const char *token) {
    if (streq("IF", token)) {
        return IF_EQ;
    } else if (streq("NIF", token)) {
        return IF_NE;
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
    token->indent = tok->indent;

    tok->next = token;

    while ((c = getc(stream)) != EOF) {
        switch (c) {
            case '\n':
            case ' ':
                if (c == ' ' && tmpi == 0) {
                    break;
                }
                if (!in_quotes && !in_dquotes){
                    tmpl[tmpi++] = '\0';
                    token->type = gettokentype(tmpl);
                    token->data = malloc(tmpi * sizeof(char));
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

/** Follow the list backwards until we hit an [N]IF token type
 *
 *  \param[in]  head  pointer to starting location in the list
 *
 *  \return  pointer to Token conditional, NULL if not found
 */
Token *token_find_last_conditional(Token *head, int indent_level) {
    Token *t = head;
    while (t != NULL) {
        if ((t->type == IF_EQ || t->type == IF_NE) && t->indent == indent_level)
            return t;
        t = t->prev;
    }
    return NULL;
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
    for (i = 0; i <= token->indent; i++)
        printf(">");
    printf("%d ", token->index);
    switch (token->type) {
        case BEGINNING:
            printf("BEGINNING");
            break;
        case IF_EQ:
            printf("IF ");
            break;
        case IF_NE:
            printf("NIF ");
            break;
        case STR:
            printf("STR ");
            printf("%s ", token->data);
            break;
        case NEWLINE:
            printf("NEWLINE ");
            break;
        case INDENT:
            printf("INDENT ");
            break;
        case UNKNOWN:
            printf("UNKNOWN ");
            break;
    }
    printf("\n");
}
