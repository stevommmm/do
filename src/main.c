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


void parse_file(const char *filename) {
    FILE *stream;
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

    // debug dump out our list
    token_current = token_head;
    while (token_current != NULL) {
        printf("%d - %d: '%s'\n", token_current->index, token_current->type, token_current->data);
        token_current = token_current->next;
    }

    // run through structure
    token_current = token_head;
    while (token_current != NULL) {
        //
    }



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
