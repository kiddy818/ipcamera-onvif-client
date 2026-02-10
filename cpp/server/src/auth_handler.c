/**
 * @file auth_handler.c
 * @brief Authentication Handler Implementation for ONVIF Server
 */

#include "../include/auth_handler.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

/* Simple XML extraction helper */
static const char* extract_value(const char* xml, const char* tag, 
                                 char* value, size_t max_len) {
    char open_tag[256];
    char close_tag[256];
    snprintf(open_tag, sizeof(open_tag), "<%s>", tag);
    snprintf(close_tag, sizeof(close_tag), "</%s>", tag);
    
    const char* start = strstr(xml, open_tag);
    if (!start) {
        return NULL;
    }
    start += strlen(open_tag);
    
    const char* end = strstr(start, close_tag);
    if (!end || end <= start) {
        return NULL;
    }
    
    size_t len = end - start;
    if (len >= max_len) {
        len = max_len - 1;
    }
    
    strncpy(value, start, len);
    value[len] = '\0';
    
    return end + strlen(close_tag);
}

/* Base64 decode helper */
static int base64_decode(const char* input, unsigned char* output, size_t* out_len) {
    BIO *bio, *b64;
    int decode_len = strlen(input);
    
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf((void*)input, decode_len);
    bio = BIO_push(b64, bio);
    
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    *out_len = BIO_read(bio, output, decode_len);
    
    BIO_free_all(bio);
    
    return (*out_len > 0) ? 0 : -1;
}

int auth_extract_token(const char* soap_header, ws_username_token_t* token) {
    if (!soap_header || !token) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    memset(token, 0, sizeof(ws_username_token_t));
    
    /* Extract username */
    if (!extract_value(soap_header, "Username", token->username, 
                      sizeof(token->username))) {
        return ONVIF_SERVER_ERR_PARSE;
    }
    
    /* Extract password */
    if (!extract_value(soap_header, "Password", token->password,
                      sizeof(token->password))) {
        return ONVIF_SERVER_ERR_PARSE;
    }
    
    /* Extract nonce (optional) */
    extract_value(soap_header, "Nonce", token->nonce, sizeof(token->nonce));
    
    /* Extract created timestamp (optional) */
    extract_value(soap_header, "Created", token->created, sizeof(token->created));
    
    /* Check if password is digest */
    token->is_digest = (strstr(soap_header, "PasswordDigest") != NULL);
    
    return ONVIF_SERVER_OK;
}

int auth_validate_token(onvif_server_t* server, const ws_username_token_t* token) {
    if (!server || !token) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    /* Find user */
    onvif_user_t* user = NULL;
    for (int i = 0; i < server->user_count; i++) {
        if (strcmp(server->users[i].username, token->username) == 0) {
            user = &server->users[i];
            break;
        }
    }
    
    if (!user || !user->enabled) {
        return ONVIF_SERVER_ERR_AUTH;
    }
    
    /* Validate timestamp if present */
    if (token->created[0]) {
        if (!auth_validate_timestamp(token->created)) {
            return ONVIF_SERVER_ERR_AUTH;
        }
    }
    
    /* Check nonce for replay attacks */
    if (token->nonce[0]) {
        if (!auth_check_nonce(server, token->nonce)) {
            return ONVIF_SERVER_ERR_AUTH;
        }
        auth_add_nonce(server, token->nonce);
    }
    
    /* Validate password */
    if (token->is_digest) {
        if (!auth_validate_digest(token->password, token->nonce,
                                 token->created, user->password)) {
            return ONVIF_SERVER_ERR_AUTH;
        }
    } else {
        /* Plain text password */
        if (strcmp(token->password, user->password) != 0) {
            return ONVIF_SERVER_ERR_AUTH;
        }
    }
    
    return ONVIF_SERVER_OK;
}

bool auth_validate_digest(const char* digest_from_client,
                         const char* nonce,
                         const char* created,
                         const char* password) {
    if (!digest_from_client || !nonce || !created || !password) {
        return false;
    }
    
    /* Decode nonce from base64 */
    unsigned char nonce_decoded[128];
    size_t nonce_len;
    if (base64_decode(nonce, nonce_decoded, &nonce_len) != 0) {
        return false;
    }
    
    /* Calculate digest: Base64(SHA1(nonce + created + password)) */
    unsigned char digest_data[1024];
    size_t digest_data_len = 0;
    
    memcpy(digest_data, nonce_decoded, nonce_len);
    digest_data_len += nonce_len;
    
    size_t created_len = strlen(created);
    memcpy(digest_data + digest_data_len, created, created_len);
    digest_data_len += created_len;
    
    size_t password_len = strlen(password);
    memcpy(digest_data + digest_data_len, password, password_len);
    digest_data_len += password_len;
    
    /* Calculate SHA1 */
    unsigned char sha1_result[SHA_DIGEST_LENGTH];
    SHA1(digest_data, digest_data_len, sha1_result);
    
    /* Base64 encode */
    BIO *bio, *b64;
    BUF_MEM *buffer_ptr;
    
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, sha1_result, SHA_DIGEST_LENGTH);
    BIO_flush(bio);
    
    BIO_get_mem_ptr(bio, &buffer_ptr);
    
    char calculated_digest[256];
    size_t encoded_len = buffer_ptr->length;
    if (encoded_len >= sizeof(calculated_digest)) {
        encoded_len = sizeof(calculated_digest) - 1;
    }
    memcpy(calculated_digest, buffer_ptr->data, encoded_len);
    calculated_digest[encoded_len] = '\0';
    
    BIO_free_all(bio);
    
    /* Compare digests */
    return (strcmp(digest_from_client, calculated_digest) == 0);
}

bool auth_validate_timestamp(const char* created_timestamp) {
    if (!created_timestamp || !created_timestamp[0]) {
        return false;
    }
    
    /* Parse ISO 8601 timestamp: 2024-01-01T12:00:00Z */
    struct tm tm_created;
    memset(&tm_created, 0, sizeof(struct tm));
    
    int year, month, day, hour, minute, second;
    if (sscanf(created_timestamp, "%d-%d-%dT%d:%d:%dZ",
               &year, &month, &day, &hour, &minute, &second) != 6) {
        return false;
    }
    
    /* Validate date/time components */
    if (year < 1900 || year > 3000) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if (hour < 0 || hour > 23) return false;
    if (minute < 0 || minute > 59) return false;
    if (second < 0 || second > 59) return false;
    
    tm_created.tm_year = year - 1900;
    tm_created.tm_mon = month - 1;
    tm_created.tm_mday = day;
    tm_created.tm_hour = hour;
    tm_created.tm_min = minute;
    tm_created.tm_sec = second;
    
    time_t created_time = timegm(&tm_created);
    time_t now = time(NULL);
    
    /* Check if timestamp is within tolerance */
    time_t diff = now - created_time;
    if (diff < 0) diff = -diff;
    
    return (diff <= ONVIF_TIMESTAMP_TOLERANCE_SEC);
}

bool auth_check_nonce(onvif_server_t* server, const char* nonce) {
    if (!server || !nonce) {
        return false;
    }
    
    /* Check if nonce already exists in cache */
    for (int i = 0; i < ONVIF_NONCE_CACHE_SIZE; i++) {
        if (server->nonce_cache[i].used &&
            strcmp(server->nonce_cache[i].nonce, nonce) == 0) {
            /* Nonce already used */
            return false;
        }
    }
    
    return true;
}

void auth_add_nonce(onvif_server_t* server, const char* nonce) {
    if (!server || !nonce) {
        return;
    }
    
    int index = server->nonce_cache_index;
    
    strncpy(server->nonce_cache[index].nonce, nonce,
            sizeof(server->nonce_cache[index].nonce) - 1);
    server->nonce_cache[index].timestamp = time(NULL);
    server->nonce_cache[index].used = true;
    
    server->nonce_cache_index = (index + 1) % ONVIF_NONCE_CACHE_SIZE;
}
