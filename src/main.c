#include <glob.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>   // pid_t for old posix versions

#include "token.h"

/** fork & wait for executed command character array
 *
 *  \param[in]  command  pointer array of strings to exec, requires NULL terminator
 *
 *  \return  exit status of child process executing command
 */
int exec_cmd(char **command) {
    pid_t pid;
    int status;
    int exit_status = 1;

    if ((pid = fork()) == 0) {
        execvp(command[0], command);
        perror("fork");
        exit(EXIT_FAILURE);
    }

    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        exit_status = WEXITSTATUS(status);
    }

    int i = 0;
    char *d = command[i];
    printf("\x1b[30m>");
    while (d != NULL) {
        printf(" %s", d);
        d = command[++i];
    }
    printf("\t< code:%d\x1b[0m\n", exit_status);
    return exit_status;
}

/** Build an array of strings from our linked list to exec
 *
 *  \param[in]  cond  pointer conditional Token
 *  \param[in]  nl    pointer to NEWLINE Token (used as null terminator in exec)
 *
 *  \return  exit status of executed command
 */
int extract_cmd(Token *cond, Token *nl) {
    // size out the exec array
    int command_elements = nl->index - cond->index;

    // build out an array of the right size, NEWLINE is the terminating NULL
    int i;
    char *command[command_elements];

    for (i = 0; i < command_elements; i++) {
        cond = cond->next;
        command[i] = cond->data;
    }
    return exec_cmd(command);
}

/** Process the file given using our dll tokenizer
 *
 *  \param[in]  filename  the path/name of our file to execute
 */
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

    // Build up our dll iteratively - can neaten this up
    while ((token_current = gettoken(stream, token_current)) != NULL);

    // debug dump out our list
    token_current = token_head;
    while (token_current != NULL) {
        printf("%d - %d: '%s'\n", token_current->index, token_current->type, token_current->data);
        token_current = token_current->next;
    }

    printf("-------------------\n");

    // run through structure
    token_current = token_head;
    Token *tmpt;
    while (token_current != NULL) {
        switch (token_current->type) {
            case IF_EQ:
            case IF_NE:
                tmpt = token_find_next_of(token_current, NEWLINE);
                if (tmpt == NULL)
                    break;

                // Save the result on the IF/NIF for trackback from indenting
                if (token_current->type == IF_EQ) {
                    token_current->passed = (bool) extract_cmd(token_current, tmpt) == 0;
                } else {
                    token_current->passed = (bool) extract_cmd(token_current, tmpt) != 0;
                }
                // jump forwards again so we dont process all the STR making up the command
                token_current = tmpt;

                break;
            case INDENT:
                tmpt = token_find_last_conditional(token_current);
                if (tmpt == NULL)
                    break;

                // As long as a conditional passed we'll run
                if (tmpt->passed) {
                    tmpt = token_find_next_of(token_current, NEWLINE);
                    if (tmpt == NULL)
                        break;

                    extract_cmd(token_current, tmpt); // throw away the result, we don't care
                    token_current = tmpt;
                }
                break;
        }
        token_current = token_current->next;
    }

    fclose(stream);
    token_follow_free(token_head);
}

/** Glob over the given directory for matching files & process them iteratively
 *
 *  \param[in]  script_path  a globbable pattern
 */
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
