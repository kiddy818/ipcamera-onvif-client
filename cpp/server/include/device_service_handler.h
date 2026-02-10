/**
 * @file device_service_handler.h
 * @brief ONVIF Device Management Service Handler
 */

#ifndef DEVICE_SERVICE_HANDLER_H
#define DEVICE_SERVICE_HANDLER_H

#include "onvif_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handle GetDeviceInformation request
 * 
 * @param server Pointer to server structure
 * @param response_xml Buffer for response XML
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int handle_get_device_information(onvif_server_t* server,
                                  char* response_xml,
                                  size_t max_len);

/**
 * @brief Handle GetCapabilities request
 * 
 * @param server Pointer to server structure
 * @param response_xml Buffer for response XML
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int handle_get_capabilities(onvif_server_t* server,
                            char* response_xml,
                            size_t max_len);

/**
 * @brief Handle GetServices request
 * 
 * @param server Pointer to server structure
 * @param response_xml Buffer for response XML
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int handle_get_services(onvif_server_t* server,
                       char* response_xml,
                       size_t max_len);

/**
 * @brief Handle GetSystemDateAndTime request
 * 
 * @param server Pointer to server structure
 * @param response_xml Buffer for response XML
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int handle_get_system_date_time(onvif_server_t* server,
                                char* response_xml,
                                size_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_SERVICE_HANDLER_H */
