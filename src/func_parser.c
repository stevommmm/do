#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "func_parser.h"


int cmd_count_args(const char *line) {
    int count = 0;
    int i;

    bool in_quotes = false;
    bool in_dquotes = false;

    for (i = 0; i < strlen(line); i++) {
        switch (line[i]) {
            case ' ':
                if (!in_quotes && !in_dquotes){
                    count++;
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
        }
    }
    return ++count; // extra for NULL terminator
}


func_args *cmd_split_line(const char *line) {
    func_args *fa;
    fa = (func_args*)malloc(sizeof(func_args));
    fa->count = cmd_count_args(line);
    fa->data = malloc((fa->count + 1) * sizeof(char *));
    int data_i = 0;

    char tmp[strlen(line)];
    int tmp_i = 0;

    int i;
    bool in_quotes = false;
    bool in_dquotes = false;

    for (i = 0; i < strlen(line); i++) {
        switch (line[i]) {
            case ' ':
                if (!in_quotes && !in_dquotes) {
                    tmp[tmp_i] = '\0';
                    fa->data[data_i++] = strndup(tmp, tmp_i);
                    tmp_i = 0;
                } else{
                    tmp[tmp_i++] = line[i];
                }
                break;
            case '\'':
                if (in_dquotes){
                    tmp[tmp_i++] = line[i];
                    break;
                }
                in_quotes = !in_quotes;
                break;
            case '"':
                if (in_quotes){
                    tmp[tmp_i++] = line[i];
                    break;
                }
                in_dquotes = !in_dquotes;
                break;
            case '\0':
                break;
            default:
                tmp[tmp_i++] = line[i];
                break;
        }
    }
    // Whatever is left in our buffer becomes an arg
    if (tmp_i > 0) {
        fa->data[data_i++] = strndup(tmp, tmp_i);
    }

    fa->data[data_i] = NULL;
    return fa;
}


void free_func_args(func_args *fa) {
    int i;
    for (i = 0; i < fa->count; i++) {
        free(fa->data[i]);
    }
    free(fa->data);
    free(fa);
}
