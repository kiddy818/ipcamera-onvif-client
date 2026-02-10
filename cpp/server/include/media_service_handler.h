/**
 * @file media_service_handler.h
 * @brief ONVIF Media Service Handler
 */

#ifndef MEDIA_SERVICE_HANDLER_H
#define MEDIA_SERVICE_HANDLER_H

#include "onvif_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handle GetProfiles request
 * 
 * @param server Pointer to server structure
 * @param response_xml Buffer for response XML
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int handle_get_profiles(onvif_server_t* server,
                       char* response_xml,
                       size_t max_len);

/**
 * @brief Handle GetStreamUri request
 * 
 * @param server Pointer to server structure
 * @param profile_token Profile token from request
 * @param response_xml Buffer for response XML
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int handle_get_stream_uri(onvif_server_t* server,
                         const char* profile_token,
                         char* response_xml,
                         size_t max_len);

/**
 * @brief Handle GetSnapshotUri request
 * 
 * @param server Pointer to server structure
 * @param profile_token Profile token from request
 * @param response_xml Buffer for response XML
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int handle_get_snapshot_uri(onvif_server_t* server,
                           const char* profile_token,
                           char* response_xml,
                           size_t max_len);

/**
 * @brief Handle GetVideoEncoderConfiguration request
 * 
 * @param server Pointer to server structure
 * @param config_token Configuration token from request
 * @param response_xml Buffer for response XML
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int handle_get_video_encoder_configuration(onvif_server_t* server,
                                          const char* config_token,
                                          char* response_xml,
                                          size_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* MEDIA_SERVICE_HANDLER_H */
