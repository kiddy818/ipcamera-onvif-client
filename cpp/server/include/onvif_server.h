/**
 * @file onvif_server.h
 * @brief ONVIF Profile S Server Implementation
 * 
 * This header defines the main API for the ONVIF server implementation.
 * The server provides basic SOAP message handling and optionally supports
 * gSOAP for enhanced SOAP processing when compiled with USE_GSOAP=ON.
 * The implementation follows BDD approach with comprehensive feature testing.
 */

#ifndef ONVIF_SERVER_H
#define ONVIF_SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes */
#define ONVIF_SERVER_OK                 0
#define ONVIF_SERVER_ERR_INVALID_PARAM -1
#define ONVIF_SERVER_ERR_NETWORK       -2
#define ONVIF_SERVER_ERR_AUTH          -3
#define ONVIF_SERVER_ERR_PARSE         -4
#define ONVIF_SERVER_ERR_NOT_SUPPORTED -5
#define ONVIF_SERVER_ERR_MEMORY        -6
#define ONVIF_SERVER_ERR_ALREADY_RUNNING -7

/* Configuration constants */
#define ONVIF_MAX_USERS                 10
#define ONVIF_MAX_USERNAME_LEN          64
#define ONVIF_MAX_PASSWORD_LEN          64
#define ONVIF_MAX_STRING_LEN            256
#define ONVIF_MAX_URL_LEN               512
#define ONVIF_MAX_PROFILES              10
#define ONVIF_NONCE_CACHE_SIZE          100
#define ONVIF_TIMESTAMP_TOLERANCE_SEC   300  // 5 minutes
#define ONVIF_MAX_REQUEST_SIZE          65536 // 64KB max request size

/* Forward declarations */
struct soap;

/**
 * @brief User credentials for authentication
 */
typedef struct {
    char username[ONVIF_MAX_USERNAME_LEN];
    char password[ONVIF_MAX_PASSWORD_LEN];
    bool enabled;
} onvif_user_t;

/**
 * @brief Device information
 */
typedef struct {
    char manufacturer[ONVIF_MAX_STRING_LEN];
    char model[ONVIF_MAX_STRING_LEN];
    char firmware_version[ONVIF_MAX_STRING_LEN];
    char serial_number[ONVIF_MAX_STRING_LEN];
    char hardware_id[ONVIF_MAX_STRING_LEN];
} onvif_device_info_t;

/**
 * @brief Media profile configuration
 */
typedef struct {
    char token[ONVIF_MAX_STRING_LEN];
    char name[ONVIF_MAX_STRING_LEN];
    bool fixed;
    
    /* Video encoder configuration */
    char encoding[32];           // H264, MJPEG, etc.
    int width;
    int height;
    int frame_rate_limit;
    int quality;
    int bitrate_limit;
    
    /* Stream URIs */
    char rtsp_uri[ONVIF_MAX_URL_LEN];
    char snapshot_uri[ONVIF_MAX_URL_LEN];
} onvif_media_profile_t;

/**
 * @brief Service endpoint information
 */
typedef struct {
    char namespace[ONVIF_MAX_URL_LEN];
    char xaddr[ONVIF_MAX_URL_LEN];
    int version_major;
    int version_minor;
} onvif_service_t;

/**
 * @brief Nonce cache entry for replay attack prevention
 */
typedef struct {
    char nonce[64];
    time_t timestamp;
    bool used;
} nonce_cache_entry_t;

/**
 * @brief ONVIF server configuration
 */
typedef struct {
    /* Network settings */
    int port;
    char bind_address[64];
    int max_connections;
    int timeout_sec;
    
    /* Device information */
    onvif_device_info_t device_info;
    
    /* Authentication */
    bool require_auth;
    onvif_user_t users[ONVIF_MAX_USERS];
    int user_count;
    
    /* Nonce cache for replay protection */
    nonce_cache_entry_t nonce_cache[ONVIF_NONCE_CACHE_SIZE];
    int nonce_cache_index;
    
    /* Media profiles */
    onvif_media_profile_t profiles[ONVIF_MAX_PROFILES];
    int profile_count;
    
    /* Services */
    onvif_service_t device_service;
    onvif_service_t media_service;
    onvif_service_t ptz_service;
    bool ptz_enabled;
    
    /* Server state */
    bool running;
    struct soap* soap_context;
} onvif_server_t;

/**
 * @brief Initialize ONVIF server with default configuration
 * 
 * @param server Pointer to server structure
 * @param port Port number to listen on
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int onvif_server_init(onvif_server_t* server, int port);

/**
 * @brief Set device information
 * 
 * @param server Pointer to server structure
 * @param manufacturer Manufacturer name
 * @param model Model name
 * @param firmware_version Firmware version string
 * @param serial_number Serial number
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int onvif_server_set_device_info(onvif_server_t* server,
                                  const char* manufacturer,
                                  const char* model,
                                  const char* firmware_version,
                                  const char* serial_number);

/**
 * @brief Add user for authentication
 * 
 * @param server Pointer to server structure
 * @param username Username
 * @param password Password
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int onvif_server_add_user(onvif_server_t* server,
                          const char* username,
                          const char* password);

/**
 * @brief Add media profile
 * 
 * @param server Pointer to server structure
 * @param profile Pointer to media profile configuration
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int onvif_server_add_profile(onvif_server_t* server,
                             const onvif_media_profile_t* profile);

/**
 * @brief Enable or disable authentication requirement
 * 
 * @param server Pointer to server structure
 * @param require True to require authentication, false otherwise
 */
void onvif_server_set_auth_required(onvif_server_t* server, bool require);

/**
 * @brief Start the ONVIF server
 * 
 * This function starts the server and begins listening for connections.
 * It will block until the server is stopped.
 * 
 * @param server Pointer to server structure
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int onvif_server_start(onvif_server_t* server);

/**
 * @brief Stop the ONVIF server
 * 
 * @param server Pointer to server structure
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int onvif_server_stop(onvif_server_t* server);

/**
 * @brief Cleanup server resources
 * 
 * @param server Pointer to server structure
 */
void onvif_server_destroy(onvif_server_t* server);

/**
 * @brief Check if server is running
 * 
 * @param server Pointer to server structure
 * @return true if running, false otherwise
 */
bool onvif_server_is_running(const onvif_server_t* server);

#ifdef __cplusplus
}
#endif

#endif /* ONVIF_SERVER_H */
