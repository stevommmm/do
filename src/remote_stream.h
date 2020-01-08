// vim: set syntax=c et ts=4:
#ifndef REMOTE_STREAM_H
#define REMOTE_STREAM_H

#include <stdio.h>


typedef struct do_stream {
    FILE *stream;
    char *resp;
    size_t resp_len;
} do_stream;

do_stream *remote_stream(const char *url);
void remote_stream_free(do_stream *s);

#endif /* REMOTE_STREAM_H */
