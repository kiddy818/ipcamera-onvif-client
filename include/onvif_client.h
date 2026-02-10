#ifndef ONVIF_CLIENT_H
#define ONVIF_CLIENT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes */
#define ONVIF_OK                0
#define ONVIF_ERR_INVALID_PARAM -1
#define ONVIF_ERR_NETWORK       -2
#define ONVIF_ERR_AUTH          -3
#define ONVIF_ERR_PARSE         -4
#define ONVIF_ERR_NOT_SUPPORTED -5

/* Maximum lengths */
#define ONVIF_MAX_URL_LEN       256
#define ONVIF_MAX_STRING_LEN    128
#define ONVIF_MAX_PROFILES      16

/* ONVIF Client structure */
typedef struct {
    char device_url[ONVIF_MAX_URL_LEN];
    char username[ONVIF_MAX_STRING_LEN];
    char password[ONVIF_MAX_STRING_LEN];
    int timeout_ms;
} onvif_client_t;

/* Device Information */
typedef struct {
    char manufacturer[ONVIF_MAX_STRING_LEN];
    char model[ONVIF_MAX_STRING_LEN];
    char firmware_version[ONVIF_MAX_STRING_LEN];
    char serial_number[ONVIF_MAX_STRING_LEN];
    char hardware_id[ONVIF_MAX_STRING_LEN];
} onvif_device_info_t;

/* Media Profile */
typedef struct {
    char token[ONVIF_MAX_STRING_LEN];
    char name[ONVIF_MAX_STRING_LEN];
    int video_encoder_config_token;
    int video_source_config_token;
} onvif_media_profile_t;

/* Stream URI */
typedef struct {
    char uri[ONVIF_MAX_URL_LEN];
    int invalid_after_connect;
    int invalid_after_reboot;
    int timeout_sec;
} onvif_stream_uri_t;

/* Capabilities */
typedef struct {
    int device_service;
    int media_service;
    int ptz_service;
    int imaging_service;
    int analytics_service;
} onvif_capabilities_t;

/* Client functions */
int onvif_client_init(onvif_client_t* client, const char* device_url,
                      const char* username, const char* password);
void onvif_client_destroy(onvif_client_t* client);

/* Device Management Service */
int onvif_get_device_information(onvif_client_t* client, onvif_device_info_t* info);
int onvif_get_capabilities(onvif_client_t* client, onvif_capabilities_t* caps);
int onvif_get_services(onvif_client_t* client, char* services, size_t max_len);

/* Media Service */
int onvif_get_profiles(onvif_client_t* client, onvif_media_profile_t* profiles,
                       int* count, int max_profiles);
int onvif_get_stream_uri(onvif_client_t* client, const char* profile_token,
                         onvif_stream_uri_t* stream_uri);
int onvif_get_snapshot_uri(onvif_client_t* client, const char* profile_token,
                           char* uri, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* ONVIF_CLIENT_H */
