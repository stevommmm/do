// vim: set syntax=c et ts=4:
#ifndef NRDP_H
#define NRDP_H

#include <stdbool.h>

bool curl_post_xml(const char *url, const char *message);
bool nrdp_host_send(const char *url, const char *token, const char *hostname, int state, const char *output);
bool nrdp_service_send(const char *url, const char *token, const char *service, const char *hostname, int state, const char *output);

#endif /* NRDP_H */
