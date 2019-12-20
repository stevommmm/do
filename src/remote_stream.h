// vim: set syntax=c et ts=4:
#ifndef REMOTE_STREAM_H
#define REMOTE_STREAM_H

#include <stdio.h>


typedef struct ok_stream {
    FILE *stream;
    char *resp;
    size_t resp_len;
} ok_stream;

ok_stream *remote_stream(const char *url);
void remote_stream_free(ok_stream *s);

#endif /* REMOTE_STREAM_H */
