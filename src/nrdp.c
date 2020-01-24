#include <curl/curl.h>
#include <string.h>

#include "nrdp.h"

size_t suppress_wb(void *buffer, size_t size, size_t nmemb, void *userp) {
    return size * nmemb;
}

/** Build xml up for a HOST type update to Nagios NRDP endpoint
 *
 *  \param[in]  url        remote endpoint (https://nagios/nrdp/)
 *  \param[in]  token      unique authenitcation token for NRDP
 *  \param[in]  hostname   matching name in nagios
 *  \param[in]  state      1|2|3 depending on OK|WARN|ERROR
 *  \param[in]  output     freeform text to send (and perfdata)
 *
 *  \return  true if the NRDP submission succeeded, else false
 */
bool nrdp_host_send(const char *url, const char *token,
    const char *hostname, int state, const char *output) {
    char message[1024] = {'\0'};
    const char *template = "token=%s&cmd=submitcmd&command=PROCESS_HOST_CHECK_RESULT;%s;%d;%s";

    snprintf(message, 1023, template, token, hostname, state, output);
    return curl_post_xml(url, message);
}

/** Build xml up for a SERVICE type update to Nagios NRDP endpoint
 *
 *  \param[in]  url        remote endpoint (https://nagios/nrdp/)
 *  \param[in]  token      unique authenitcation token for NRDP
 *  \param[in]  service    service description from nagios to update
 *  \param[in]  hostname   matching name in nagios
 *  \param[in]  state      1|2|3 depending on OK|WARN|ERROR
 *  \param[in]  output     freeform text to send (and perfdata)
 *
 *  \return  true if the NRDP submission succeeded, else false
 */
bool nrdp_service_send(const char *url, const char *token,
    const char *service, const char *hostname, int state, const char *output) {
    char message[1024] = {'\0'};
    const char *template = "token=%s&cmd=submitcmd&command=PROCESS_SERVICE_CHECK_RESULT;%s;%s;%d;%s";

    snprintf(message, 1023, template, token, hostname, service, state, output);
    return curl_post_xml(url, message);
}

/** curl worker to POST xml
 *
 *  \param[in]  url      remote endpoint (https://nagios/nrdp/)
 *  \param[in]  message  POST data with token|cmd|xml defined
 *
 *  \return  true if the NRDP submission succeeded, else false
 */
bool curl_post_xml(const char *url, const char *message) {
    bool code = true;
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(message));
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, suppress_wb);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        fprintf(stderr, "NRDP failed: %s\n", curl_easy_strerror(res));
        code = false;
    }
    curl_easy_cleanup(curl);
    return code;
}
