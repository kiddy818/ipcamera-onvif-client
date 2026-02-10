#ifndef SOAP_HANDLER_H
#define SOAP_HANDLER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SOAP request/response structure */
typedef struct {
    char* xml_data;
    size_t xml_len;
    char* namespace_uri;
    char* action;
} soap_message_t;

/* Create SOAP envelope */
int soap_create_envelope(soap_message_t* msg, const char* namespace_uri,
                        const char* action, const char* body);

/* Parse SOAP response */
int soap_parse_response(const char* xml, size_t xml_len, soap_message_t* msg);

/* Extract value from XML */
int soap_extract_value(const char* xml, const char* tag, char* value, size_t max_len);

/* Free SOAP message */
void soap_free_message(soap_message_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* SOAP_HANDLER_H */
