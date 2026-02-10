#include "onvif_client.h"
#include "soap_handler.h"
#include "http_client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int onvif_get_device_information(onvif_client_t* client, onvif_device_info_t* info) {
    if (!client || !info) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Construct SOAP request for GetDeviceInformation */
    const char* body = 
        "<GetDeviceInformation xmlns=\"http://www.onvif.org/ver10/device/wsdl\"/>";
    
    soap_message_t request;
    int ret = soap_create_envelope(&request, 
                                   "http://www.onvif.org/ver10/device/wsdl",
                                   "http://www.onvif.org/ver10/device/wsdl/GetDeviceInformation",
                                   body);
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Send HTTP request */
    http_response_t response;
    char device_service_url[ONVIF_MAX_URL_LEN];
    snprintf(device_service_url, sizeof(device_service_url), 
             "%s/onvif/device_service", client->device_url);
    
    ret = http_post(device_service_url, request.xml_data, request.xml_len,
                   client->username, client->password, &response, client->timeout_ms);
    
    soap_free_message(&request);
    
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Parse response */
    memset(info, 0, sizeof(onvif_device_info_t));
    soap_extract_value(response.body, "Manufacturer", info->manufacturer, 
                      ONVIF_MAX_STRING_LEN);
    soap_extract_value(response.body, "Model", info->model, 
                      ONVIF_MAX_STRING_LEN);
    soap_extract_value(response.body, "FirmwareVersion", info->firmware_version, 
                      ONVIF_MAX_STRING_LEN);
    soap_extract_value(response.body, "SerialNumber", info->serial_number, 
                      ONVIF_MAX_STRING_LEN);
    soap_extract_value(response.body, "HardwareId", info->hardware_id, 
                      ONVIF_MAX_STRING_LEN);
    
    http_free_response(&response);
    
    return ONVIF_OK;
}

int onvif_get_capabilities(onvif_client_t* client, onvif_capabilities_t* caps) {
    if (!client || !caps) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Construct SOAP request for GetCapabilities */
    const char* body = 
        "<GetCapabilities xmlns=\"http://www.onvif.org/ver10/device/wsdl\">"
        "<Category>All</Category>"
        "</GetCapabilities>";
    
    soap_message_t request;
    int ret = soap_create_envelope(&request,
                                   "http://www.onvif.org/ver10/device/wsdl",
                                   "http://www.onvif.org/ver10/device/wsdl/GetCapabilities",
                                   body);
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Send HTTP request */
    http_response_t response;
    char device_service_url[ONVIF_MAX_URL_LEN];
    snprintf(device_service_url, sizeof(device_service_url),
             "%s/onvif/device_service", client->device_url);
    
    ret = http_post(device_service_url, request.xml_data, request.xml_len,
                   client->username, client->password, &response, client->timeout_ms);
    
    soap_free_message(&request);
    
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Parse response - check for service availability */
    memset(caps, 0, sizeof(onvif_capabilities_t));
    
    /* Simple parsing - in real implementation would parse XML properly */
    caps->device_service = (strstr(response.body, "Device") != NULL) ? 1 : 0;
    caps->media_service = (strstr(response.body, "Media") != NULL) ? 1 : 0;
    caps->ptz_service = (strstr(response.body, "PTZ") != NULL) ? 1 : 0;
    caps->imaging_service = (strstr(response.body, "Imaging") != NULL) ? 1 : 0;
    caps->analytics_service = (strstr(response.body, "Analytics") != NULL) ? 1 : 0;
    
    http_free_response(&response);
    
    return ONVIF_OK;
}

int onvif_get_services(onvif_client_t* client, char* services, size_t max_len) {
    if (!client || !services || max_len == 0) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Construct SOAP request for GetServices */
    const char* body = 
        "<GetServices xmlns=\"http://www.onvif.org/ver10/device/wsdl\">"
        "<IncludeCapability>false</IncludeCapability>"
        "</GetServices>";
    
    soap_message_t request;
    int ret = soap_create_envelope(&request,
                                   "http://www.onvif.org/ver10/device/wsdl",
                                   "http://www.onvif.org/ver10/device/wsdl/GetServices",
                                   body);
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Send HTTP request */
    http_response_t response;
    char device_service_url[ONVIF_MAX_URL_LEN];
    snprintf(device_service_url, sizeof(device_service_url),
             "%s/onvif/device_service", client->device_url);
    
    ret = http_post(device_service_url, request.xml_data, request.xml_len,
                   client->username, client->password, &response, client->timeout_ms);
    
    soap_free_message(&request);
    
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Copy response to output buffer */
    strncpy(services, response.body, max_len - 1);
    services[max_len - 1] = '\0';
    
    http_free_response(&response);
    
    return ONVIF_OK;
}
