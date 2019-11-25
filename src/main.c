#include <glob.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "func_parser.h"
#include "token.h"

#define TOK_EQ '?'
#define TOK_NE '!'
#define TOK_COMMENT '#'
#define TOK_INDENT '\t'

int exec_cmd(char *command) {
    pid_t pid;
    int status;
    int exit_status = 1;
    func_args *fa;

    fa = cmd_split_line(command);

    if ((pid = fork()) == 0) {
        execvp(fa->data[0], fa->data);
        perror("fork");
        free_func_args(fa);
        exit(EXIT_FAILURE);
    }

    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        exit_status = WEXITSTATUS(status);
    }
    // printf("Child status: %d\n", exit_status);
    free_func_args(fa);
    return exit_status;
}

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
    char tmpl[1024] = {'\0'};

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
                if (in_dquotes)
                    break;
                in_quotes = !in_quotes;
                break;
            case '"':
                if (in_quotes)
                    break;
                in_dquotes = !in_dquotes;
                break;
            default:
                tmpl[tmpi++] = c;
        }
    }
    return NULL;
}


void parse_file(const char *filename) {
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    bool allow_indent = false;
    Token *token_head, *token_current;

    token_head = malloc(sizeof(Token));
    token_head->index = 0;
    token_head->type = BEGINNING;
    token_head->data = NULL;
    token_head->next = NULL;
    token_head->prev = NULL;

    // Keep a reference to where we are
    token_current = token_head;

    stream = fopen(filename, "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while ((token_current = gettoken(stream, token_current)) != NULL);

    token_current = token_head;
    while (token_current != NULL) {
        printf("%d - %d: '%s'\n", token_current->index, token_current->type, token_current->data);
        token_current = token_current->next;
    }

    free(line);
    fclose(stream);
    l_free(token_head);
    exit(0);

    while ((nread = getline(&line, &len, stream)) != -1) {
        // printf("Retrieved line of length %zu:\n", nread);
        line[strcspn(line, "\n")] = '\0'; // no newlines here

        switch(line[0]) {
            case TOK_EQ:
                allow_indent = false;
                if (exec_cmd(line + 1) == 0) {
                    allow_indent = true;
                }
                break;
            case TOK_NE:
                allow_indent = false;
                if (exec_cmd(line + 1) != 0) {
                    allow_indent = true;
                }
                break;
            case TOK_INDENT:
                if (allow_indent) {
                    exec_cmd(line + 1);
                }
                break;
            case TOK_COMMENT:
                break;
            case '\0':
                allow_indent = false;
                // empty lines are fine
                break;
            default:
                printf("Not sure what this is '%s'\n", line);
                break;
        }
    }
    free(line);
    fclose(stream);
    l_free(token_head);
}


void iter_scripts(const char *script_path) {
    glob_t results;
    int ret, i;
    ret = glob(script_path, 0, NULL, &results);

    if (ret != 0) {
        globfree(&results);
        return;
    }
    for (i = 0; i < results.gl_pathc; i++) {
        printf("> %s\n", results.gl_pathv[i]);
        parse_file(results.gl_pathv[i]);
    }
    globfree(&results);
}


int main(int argc, char *argv[]) {

    iter_scripts("scripts/*.f");

    return EXIT_SUCCESS;
}
