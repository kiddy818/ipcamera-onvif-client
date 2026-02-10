/**
 * @file auth_handler.h
 * @brief Authentication Handler for ONVIF Server
 */

#ifndef AUTH_HANDLER_H
#define AUTH_HANDLER_H

#include "onvif_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief WS-UsernameToken structure
 */
typedef struct {
    char username[ONVIF_MAX_USERNAME_LEN];
    char password[ONVIF_MAX_PASSWORD_LEN];
    char nonce[64];
    char created[64];
    bool is_digest;
} ws_username_token_t;

/**
 * @brief Extract WS-UsernameToken from SOAP header
 * 
 * @param soap_header SOAP header XML
 * @param token Output token structure
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int auth_extract_token(const char* soap_header, ws_username_token_t* token);

/**
 * @brief Validate WS-UsernameToken
 * 
 * @param server Pointer to server structure
 * @param token Token to validate
 * @return ONVIF_SERVER_OK if valid, error code otherwise
 */
int auth_validate_token(onvif_server_t* server, const ws_username_token_t* token);

/**
 * @brief Validate password digest
 * 
 * @param digest_from_client Password digest from client
 * @param nonce Nonce value
 * @param created Created timestamp
 * @param password Actual password
 * @return true if valid, false otherwise
 */
bool auth_validate_digest(const char* digest_from_client,
                         const char* nonce,
                         const char* created,
                         const char* password);

/**
 * @brief Check if timestamp is within acceptable range
 * 
 * @param created_timestamp Timestamp string in ISO 8601 format
 * @return true if valid, false if expired
 */
bool auth_validate_timestamp(const char* created_timestamp);

/**
 * @brief Check if nonce has been used (replay attack detection)
 * 
 * @param server Pointer to server structure
 * @param nonce Nonce to check
 * @return true if nonce is new, false if already used
 */
bool auth_check_nonce(onvif_server_t* server, const char* nonce);

/**
 * @brief Add nonce to cache
 * 
 * @param server Pointer to server structure
 * @param nonce Nonce to add
 */
void auth_add_nonce(onvif_server_t* server, const char* nonce);

#ifdef __cplusplus
}
#endif

#endif /* AUTH_HANDLER_H */
