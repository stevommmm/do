#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>   // pid_t for old posix versions
#include <sys/wait.h>
#include <unistd.h>

#include "remote_stream.h"
#include "token.h"

int DEBUG_LEVEL = 0;

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

    if (DEBUG_LEVEL == 0) {
        return exit_status;
    }

    int i = 0;
    char *d = command[i];
    printf("\x1b[30m%d:", exit_status);
    while (d != NULL) {
        printf(" %s", d);
        d = command[++i];
    }
    printf("\x1b[0m\n");
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
        if (cond->type == VAR && cond->value)
            command[i] = cond->value;
        else
            command[i] = cond->data;
    }
    return exec_cmd(command);
}

/** Process the given stream
 *
 *  \param[in]  stream  an open file resource
 */
int parse_stream(FILE *stream) {
    Token *token_head, *token_current, *tmpt;

    int return_code = 0;

    token_head = malloc(sizeof(Token));
    token_head->index = 0;
    token_head->indent = 0;
    token_head->type = BEGINNING;
    token_head->data = NULL;
    token_head->next = NULL;
    token_head->prev = NULL;

    // Keep a reference to where we are
    token_current = token_head;

    // Build up our dll iteratively - can neaten this up
    while ((token_current = gettoken(stream, token_current)) != NULL) {
        if (DEBUG_LEVEL > 2)
            token_print(token_current);
    }

    // run through structure
    token_current = token_head;
    while (token_current != NULL) {
        switch (token_current->type) {
            case FAILURE:
                if (token_begins_line(token_current) && token_last_cond_passed(token_current)) {
                    return_code++;
                }
                break;
            case SET:
                if (token_current->next->type != VAR) {
                    break;
                }
                if (token_begins_line(token_current) && token_last_cond_passed(token_current)) {
                    token_sub_var(token_current->next, token_current->next->next);
                    token_current = token_current->next->next;
                }
                break;
            case IF:
                if (!token_begins_line(token_current) || !token_last_cond_passed(token_current)) {
                    break;
                }
                tmpt = token_find_next_of(token_current, NEWLINE);
                if (tmpt == NULL)
                    break;

                // Save the result on the IF/NIF for trackback from indenting
                if (token_current->next->type == NOT) {
                    // If we're in a negated IF skip the NOT token
                    token_current->passed = (bool) extract_cmd(token_current->next, tmpt) != 0;
                } else {
                    token_current->passed = (bool) extract_cmd(token_current, tmpt) == 0;
                }
                // jump forwards again so we dont process all the STR making up the command
                token_current = tmpt;

                break;
            case SYNC:
                if (token_current->next->type != STR) {
                    break;
                }
                if (token_begins_line(token_current) && token_last_cond_passed(token_current)) {
                    do_stream *rstream;
                    rstream = remote_stream(token_current->next->data);
                    if (rstream != NULL) {
                        if (DEBUG_LEVEL > 1)
                            printf("stream: %s\n", token_current->next->data);
                        return_code += parse_stream(rstream->stream);
                        remote_stream_free(rstream);
                    }
                }

                // skip over the parsed args
                token_current = token_current->next->next;

                break;
            case DO:
                if (token_current->next->type != STR) {
                    break;
                }
                tmpt = NULL;
                // As long as a conditional passed we'll run
                if (token_begins_line(token_current) && token_last_cond_passed(token_current)) {
                    tmpt = token_find_next_of(token_current, NEWLINE);
                    if (tmpt == NULL)
                        break;

                    extract_cmd(token_current, tmpt); // throw away the result, we don't care
                    token_current = tmpt;
                }
                break;
            // explicitly silence things we dont handle directly
            case BEGINNING:
            case NOT:
            case STR:
            case INDENT:
            case VAR:
            case NEWLINE:
            case UNKNOWN:
                break;
        }
        token_current = token_current->next;
    }
    token_follow_free(token_head);

    return return_code;
}

/** Process the file given using our dll tokenizer
 *
 *  \param[in]  filename  the path/name of our file to execute
 */
int parse_file(const char *filename) {
    int rc = -1;
    FILE *stream = fopen(filename, "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if (DEBUG_LEVEL > 1)
        printf("file: %s\n", filename);

    rc = parse_stream(stream);

    fclose(stream);
    return rc;
}

int main(int argc, char *argv[]) {
    int rc = 0;
    int i;
    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            if (!strncmp(argv[i], "-v" , 2)) {
                DEBUG_LEVEL = strlen(argv[i]) - 1;
            } else {
                if ((rc = parse_file(argv[i])) == -1) {
                    return EXIT_FAILURE;
                }
            }
        }
    } else {
        fprintf(stderr, "Missing script file\n  usage: %s [-v...] <script ...>\n\n", argv[0]);
        return EXIT_FAILURE;
    }

    return rc;
}
