#include "http_client.h"
#include "onvif_client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

/* Callback for writing received data */
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    http_response_t* response = (http_response_t*)userp;
    
    char* ptr = realloc(response->body, response->body_len + realsize + 1);
    if (!ptr) {
        return 0;
    }
    
    response->body = ptr;
    memcpy(&(response->body[response->body_len]), contents, realsize);
    response->body_len += realsize;
    response->body[response->body_len] = '\0';
    
    return realsize;
}

int http_post(const char* url, const char* data, size_t data_len,
              const char* username, const char* password,
              http_response_t* response, int timeout_ms) {
    if (!url || !data || !response) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        return ONVIF_ERR_NETWORK;
    }

    /* Initialize response */
    response->body = malloc(1);
    response->body[0] = '\0';
    response->body_len = 0;
    response->status_code = 0;

    /* Set CURL options */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)data_len);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, (long)timeout_ms);
    
    /* Set authentication if provided */
    if (username && password) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST | CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERNAME, username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
    }
    
    /* Set headers */
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/soap+xml; charset=utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    /* Perform request */
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return ONVIF_ERR_NETWORK;
    }

    /* Get response code */
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    response->status_code = (int)http_code;

    /* Cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    return ONVIF_OK;
}

void http_free_response(http_response_t* response) {
    if (response && response->body) {
        free(response->body);
        response->body = NULL;
        response->body_len = 0;
        response->status_code = 0;
    }
}
