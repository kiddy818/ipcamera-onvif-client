#include "soap_handler.h"
#include "onvif_client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int soap_create_envelope(soap_message_t* msg, const char* namespace_uri,
                        const char* action, const char* body) {
    if (!msg || !namespace_uri || !action || !body) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Calculate required buffer size */
    size_t envelope_size = 1024 + strlen(body);
    msg->xml_data = (char*)malloc(envelope_size);
    if (!msg->xml_data) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Create SOAP envelope */
    int len = snprintf(msg->xml_data, envelope_size,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" "
        "xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\" "
        "xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\" "
        "xmlns:tt=\"http://www.onvif.org/ver10/schema\">"
        "<s:Header/>"
        "<s:Body>%s</s:Body>"
        "</s:Envelope>", body);
    
    msg->xml_len = len;
    msg->namespace_uri = strdup(namespace_uri);
    msg->action = strdup(action);
    
    return ONVIF_OK;
}

int soap_parse_response(const char* xml, size_t xml_len, soap_message_t* msg) {
    if (!xml || !msg || xml_len == 0) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Simple implementation - in production would use proper XML parser */
    msg->xml_data = (char*)malloc(xml_len + 1);
    if (!msg->xml_data) {
        return ONVIF_ERR_INVALID_PARAM;
    }
    
    memcpy(msg->xml_data, xml, xml_len);
    msg->xml_data[xml_len] = '\0';
    msg->xml_len = xml_len;
    
    return ONVIF_OK;
}

int soap_extract_value(const char* xml, const char* tag, char* value, size_t max_len) {
    if (!xml || !tag || !value || max_len == 0) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Simple tag-based extraction - in production would use proper XML parser */
    char open_tag[128];
    char close_tag[128];
    snprintf(open_tag, sizeof(open_tag), "<%s>", tag);
    snprintf(close_tag, sizeof(close_tag), "</%s>", tag);
    
    const char* start = strstr(xml, open_tag);
    if (!start) {
        value[0] = '\0';
        return ONVIF_ERR_PARSE;
    }
    
    start += strlen(open_tag);
    const char* end = strstr(start, close_tag);
    if (!end) {
        value[0] = '\0';
        return ONVIF_ERR_PARSE;
    }
    
    size_t len = end - start;
    if (len >= max_len) {
        len = max_len - 1;
    }
    
    memcpy(value, start, len);
    value[len] = '\0';
    
    return ONVIF_OK;
}

void soap_free_message(soap_message_t* msg) {
    if (msg) {
        if (msg->xml_data) {
            free(msg->xml_data);
            msg->xml_data = NULL;
        }
        if (msg->namespace_uri) {
            free(msg->namespace_uri);
            msg->namespace_uri = NULL;
        }
        if (msg->action) {
            free(msg->action);
            msg->action = NULL;
        }
        msg->xml_len = 0;
    }
}
