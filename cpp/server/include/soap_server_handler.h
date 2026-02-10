/**
 * @file soap_server_handler.h
 * @brief SOAP Message Handler for ONVIF Server
 */

#ifndef SOAP_SERVER_HANDLER_H
#define SOAP_SERVER_HANDLER_H

#include "onvif_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parse incoming SOAP request
 * 
 * @param request_xml SOAP request XML
 * @param action Output buffer for SOAP action
 * @param action_len Maximum action buffer length
 * @param body Output buffer for SOAP body content
 * @param body_len Maximum body buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int soap_parse_request(const char* request_xml,
                      char* action,
                      size_t action_len,
                      char* body,
                      size_t body_len);

/**
 * @brief Create SOAP response envelope
 * 
 * @param body_xml Response body XML content
 * @param response_xml Output buffer for complete SOAP response
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int soap_create_response(const char* body_xml,
                        char* response_xml,
                        size_t max_len);

/**
 * @brief Create SOAP fault response
 * 
 * @param fault_code SOAP fault code
 * @param fault_string SOAP fault string (error description)
 * @param response_xml Output buffer for SOAP fault response
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int soap_create_fault(const char* fault_code,
                     const char* fault_string,
                     char* response_xml,
                     size_t max_len);

/**
 * @brief Validate SOAP envelope structure
 * 
 * @param request_xml SOAP request XML
 * @return ONVIF_SERVER_OK if valid, error code otherwise
 */
int soap_validate_envelope(const char* request_xml);

/**
 * @brief Extract SOAP header
 * 
 * @param request_xml SOAP request XML
 * @param header Output buffer for SOAP header
 * @param max_len Maximum buffer length
 * @return ONVIF_SERVER_OK on success, error code otherwise
 */
int soap_extract_header(const char* request_xml,
                       char* header,
                       size_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* SOAP_SERVER_HANDLER_H */
