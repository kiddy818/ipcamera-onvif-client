#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HTTP response structure */
typedef struct {
    char* body;
    size_t body_len;
    int status_code;
} http_response_t;

/* Send HTTP POST request */
int http_post(const char* url, const char* data, size_t data_len,
              const char* username, const char* password,
              http_response_t* response, int timeout_ms);

/* Free HTTP response */
void http_free_response(http_response_t* response);

#ifdef __cplusplus
}
#endif

#endif /* HTTP_CLIENT_H */
