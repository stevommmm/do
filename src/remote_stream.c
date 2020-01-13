#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <curl/curl.h>

#include "remote_stream.h"

do_stream *remote_stream(const char *url) {
    do_stream *s = malloc(sizeof(do_stream));

    s->stream = open_memstream(&s->resp, &s->resp_len);

    /* get a curl handle */
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) s->stream);

    CURLcode res = curl_easy_perform(curl);
    fflush(s->stream);
    /* Check for errors */
    if(res != CURLE_OK){
        fprintf(stderr, "SYNC failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        remote_stream_free(s);
        return NULL;
    }
    curl_easy_cleanup(curl);
    return s;
}

void remote_stream_free(do_stream *s) {
    fclose(s->stream);
    free(s->resp);
    free(s);
}
