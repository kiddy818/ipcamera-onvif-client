#include "onvif_client.h"
#include "soap_handler.h"
#include "http_client.h"
#include <string.h>
#include <stdio.h>

/* PTZ Service - Basic implementation for Profile S */

/* Note: PTZ is optional in Profile S, but included for completeness */
int onvif_ptz_get_configurations(onvif_client_t* client, char* configs, size_t max_len) {
    if (!client || !configs || max_len == 0) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Construct SOAP request for GetConfigurations */
    const char* body = 
        "<GetConfigurations xmlns=\"http://www.onvif.org/ver20/ptz/wsdl\"/>";
    
    soap_message_t request;
    int ret = soap_create_envelope(&request,
                                   "http://www.onvif.org/ver20/ptz/wsdl",
                                   "http://www.onvif.org/ver20/ptz/wsdl/GetConfigurations",
                                   body);
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Send HTTP request */
    http_response_t response;
    char ptz_service_url[ONVIF_MAX_URL_LEN];
    snprintf(ptz_service_url, sizeof(ptz_service_url),
             "%s/onvif/ptz_service", client->device_url);
    
    ret = http_post(ptz_service_url, request.xml_data, request.xml_len,
                   client->username, client->password, &response, client->timeout_ms);
    
    soap_free_message(&request);
    
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Copy response to output buffer */
    strncpy(configs, response.body, max_len - 1);
    configs[max_len - 1] = '\0';
    
    http_free_response(&response);
    
    return ONVIF_OK;
}
