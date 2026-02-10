/**
 * @file soap_server_handler.c
 * @brief SOAP Message Handler Implementation for ONVIF Server
 */

#include "../include/soap_server_handler.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Simple XML helper functions */
static const char* find_tag(const char* xml, const char* tag) {
    char open_tag[256];
    snprintf(open_tag, sizeof(open_tag), "<%s", tag);
    return strstr(xml, open_tag);
}

static const char* extract_tag_content(const char* xml, const char* tag,
                                       char* content, size_t max_len) {
    char open_tag[256];
    char close_tag[256];
    snprintf(open_tag, sizeof(open_tag), "<%s>", tag);
    snprintf(close_tag, sizeof(close_tag), "</%s>", tag);
    
    const char* start = strstr(xml, open_tag);
    if (!start) {
        /* Try with attributes */
        snprintf(open_tag, sizeof(open_tag), "<%s ", tag);
        start = strstr(xml, open_tag);
        if (!start) {
            return NULL;
        }
        /* Find end of opening tag */
        start = strchr(start, '>');
        if (!start) {
            return NULL;
        }
        start++;
    } else {
        start += strlen(open_tag);
    }
    
    const char* end = strstr(start, close_tag);
    if (!end || end <= start) {
        return NULL;
    }
    
    size_t len = end - start;
    if (len >= max_len) {
        len = max_len - 1;
    }
    
    strncpy(content, start, len);
    content[len] = '\0';
    
    return end + strlen(close_tag);
}

int soap_parse_request(const char* request_xml,
                      char* action,
                      size_t action_len,
                      char* body,
                      size_t body_len) {
    if (!request_xml || !action || !body) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    /* Find Body element */
    const char* body_start = find_tag(request_xml, "s:Body");
    if (!body_start) {
        body_start = find_tag(request_xml, "soap:Body");
    }
    if (!body_start) {
        body_start = find_tag(request_xml, "Body");
    }
    
    if (!body_start) {
        return ONVIF_SERVER_ERR_PARSE;
    }
    
    /* Move to content after opening tag */
    body_start = strchr(body_start, '>');
    if (!body_start) {
        return ONVIF_SERVER_ERR_PARSE;
    }
    body_start++;
    
    /* Find closing Body tag */
    const char* body_end = strstr(body_start, "</s:Body>");
    if (!body_end) {
        body_end = strstr(body_start, "</soap:Body>");
    }
    if (!body_end) {
        body_end = strstr(body_start, "</Body>");
    }
    
    if (!body_end) {
        return ONVIF_SERVER_ERR_PARSE;
    }
    
    /* Copy body content */
    size_t len = body_end - body_start;
    if (len >= body_len) {
        len = body_len - 1;
    }
    strncpy(body, body_start, len);
    body[len] = '\0';
    
    /* Extract action from body (first element name) */
    /* Skip whitespace */
    const char* action_start = body;
    while (*action_start && isspace(*action_start)) {
        action_start++;
    }
    
    if (*action_start == '<') {
        action_start++;
        const char* action_end = action_start;
        while (*action_end && *action_end != '>' && *action_end != ' ') {
            action_end++;
        }
        
        len = action_end - action_start;
        if (len >= action_len) {
            len = action_len - 1;
        }
        strncpy(action, action_start, len);
        action[len] = '\0';
        
        /* Remove namespace prefix if present */
        char* colon = strchr(action, ':');
        if (colon) {
            memmove(action, colon + 1, strlen(colon + 1) + 1);
        }
    } else {
        action[0] = '\0';
    }
    
    return ONVIF_SERVER_OK;
}

int soap_create_response(const char* body_xml,
                        char* response_xml,
                        size_t max_len) {
    if (!body_xml || !response_xml || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    int written = snprintf(response_xml, max_len,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<s:Envelope "
        "xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" "
        "xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\" "
        "xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\" "
        "xmlns:tt=\"http://www.onvif.org/ver10/schema\">"
        "<s:Body>%s</s:Body>"
        "</s:Envelope>",
        body_xml);
    
    if (written < 0 || (size_t)written >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    return ONVIF_SERVER_OK;
}

int soap_create_fault(const char* fault_code,
                     const char* fault_string,
                     char* response_xml,
                     size_t max_len) {
    if (!fault_code || !fault_string || !response_xml || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    int written = snprintf(response_xml, max_len,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">"
        "<s:Body>"
        "<s:Fault>"
        "<s:Code>"
        "<s:Value>%s</s:Value>"
        "</s:Code>"
        "<s:Reason>"
        "<s:Text xml:lang=\"en\">%s</s:Text>"
        "</s:Reason>"
        "</s:Fault>"
        "</s:Body>"
        "</s:Envelope>",
        fault_code,
        fault_string);
    
    if (written < 0 || (size_t)written >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    return ONVIF_SERVER_OK;
}

int soap_validate_envelope(const char* request_xml) {
    if (!request_xml) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    /* Check for Envelope element */
    if (!find_tag(request_xml, "s:Envelope") &&
        !find_tag(request_xml, "soap:Envelope") &&
        !find_tag(request_xml, "Envelope")) {
        return ONVIF_SERVER_ERR_PARSE;
    }
    
    /* Check for Body element */
    if (!find_tag(request_xml, "s:Body") &&
        !find_tag(request_xml, "soap:Body") &&
        !find_tag(request_xml, "Body")) {
        return ONVIF_SERVER_ERR_PARSE;
    }
    
    return ONVIF_SERVER_OK;
}

int soap_extract_header(const char* request_xml,
                       char* header,
                       size_t max_len) {
    if (!request_xml || !header || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    /* Find Header element */
    const char* header_start = find_tag(request_xml, "s:Header");
    if (!header_start) {
        header_start = find_tag(request_xml, "soap:Header");
    }
    if (!header_start) {
        header_start = find_tag(request_xml, "Header");
    }
    
    if (!header_start) {
        /* No header found - this is valid, return empty */
        header[0] = '\0';
        return ONVIF_SERVER_OK;
    }
    
    /* Move to content after opening tag */
    header_start = strchr(header_start, '>');
    if (!header_start) {
        return ONVIF_SERVER_ERR_PARSE;
    }
    header_start++;
    
    /* Find closing Header tag */
    const char* header_end = strstr(header_start, "</s:Header>");
    if (!header_end) {
        header_end = strstr(header_start, "</soap:Header>");
    }
    if (!header_end) {
        header_end = strstr(header_start, "</Header>");
    }
    
    if (!header_end) {
        return ONVIF_SERVER_ERR_PARSE;
    }
    
    /* Copy header content */
    size_t len = header_end - header_start;
    if (len >= max_len) {
        len = max_len - 1;
    }
    strncpy(header, header_start, len);
    header[len] = '\0';
    
    return ONVIF_SERVER_OK;
}
