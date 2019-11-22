#ifndef FUNC_PARSER_H
#define FUNC_PARSER_H

typedef struct func_args {
    char **data;
    int count;
} func_args;

int cmd_count_args(const char *line);
func_args *cmd_split_line(const char *line);
void free_func_args(func_args *fa);

#endif /* FUNC_PARSER_H */
