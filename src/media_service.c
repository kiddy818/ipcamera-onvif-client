#include "onvif_client.h"
#include "soap_handler.h"
#include "http_client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int onvif_get_profiles(onvif_client_t* client, onvif_media_profile_t* profiles,
                       int* count, int max_profiles) {
    if (!client || !profiles || !count || max_profiles <= 0) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Construct SOAP request for GetProfiles */
    const char* body = 
        "<GetProfiles xmlns=\"http://www.onvif.org/ver10/media/wsdl\"/>";
    
    soap_message_t request;
    int ret = soap_create_envelope(&request,
                                   "http://www.onvif.org/ver10/media/wsdl",
                                   "http://www.onvif.org/ver10/media/wsdl/GetProfiles",
                                   body);
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Send HTTP request */
    http_response_t response;
    char media_service_url[ONVIF_MAX_URL_LEN];
    snprintf(media_service_url, sizeof(media_service_url),
             "%s/onvif/media_service", client->device_url);
    
    ret = http_post(media_service_url, request.xml_data, request.xml_len,
                   client->username, client->password, &response, client->timeout_ms);
    
    soap_free_message(&request);
    
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Parse profiles from response - simplified parsing */
    *count = 0;
    
    /* In a real implementation, would use proper XML parsing */
    /* For now, create a mock profile if response is successful */
    if (response.status_code == 200 && *count < max_profiles) {
        strncpy(profiles[0].token, "Profile_1", ONVIF_MAX_STRING_LEN - 1);
        strncpy(profiles[0].name, "MainProfile", ONVIF_MAX_STRING_LEN - 1);
        profiles[0].video_encoder_config_token = 1;
        profiles[0].video_source_config_token = 1;
        *count = 1;
    }
    
    http_free_response(&response);
    
    return ONVIF_OK;
}

int onvif_get_stream_uri(onvif_client_t* client, const char* profile_token,
                         onvif_stream_uri_t* stream_uri) {
    if (!client || !profile_token || !stream_uri) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Construct SOAP request for GetStreamUri */
    char body[512];
    snprintf(body, sizeof(body),
             "<GetStreamUri xmlns=\"http://www.onvif.org/ver10/media/wsdl\">"
             "<StreamSetup>"
             "<Stream xmlns=\"http://www.onvif.org/ver10/schema\">RTP-Unicast</Stream>"
             "<Transport xmlns=\"http://www.onvif.org/ver10/schema\">"
             "<Protocol>RTSP</Protocol>"
             "</Transport>"
             "</StreamSetup>"
             "<ProfileToken>%s</ProfileToken>"
             "</GetStreamUri>", profile_token);
    
    soap_message_t request;
    int ret = soap_create_envelope(&request,
                                   "http://www.onvif.org/ver10/media/wsdl",
                                   "http://www.onvif.org/ver10/media/wsdl/GetStreamUri",
                                   body);
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Send HTTP request */
    http_response_t response;
    char media_service_url[ONVIF_MAX_URL_LEN];
    snprintf(media_service_url, sizeof(media_service_url),
             "%s/onvif/media_service", client->device_url);
    
    ret = http_post(media_service_url, request.xml_data, request.xml_len,
                   client->username, client->password, &response, client->timeout_ms);
    
    soap_free_message(&request);
    
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Parse stream URI from response */
    memset(stream_uri, 0, sizeof(onvif_stream_uri_t));
    soap_extract_value(response.body, "Uri", stream_uri->uri, ONVIF_MAX_URL_LEN);
    
    /* Default values */
    stream_uri->invalid_after_connect = 0;
    stream_uri->invalid_after_reboot = 1;
    stream_uri->timeout_sec = 60;
    
    http_free_response(&response);
    
    return ONVIF_OK;
}

int onvif_get_snapshot_uri(onvif_client_t* client, const char* profile_token,
                           char* uri, size_t max_len) {
    if (!client || !profile_token || !uri || max_len == 0) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    /* Construct SOAP request for GetSnapshotUri */
    char body[512];
    snprintf(body, sizeof(body),
             "<GetSnapshotUri xmlns=\"http://www.onvif.org/ver10/media/wsdl\">"
             "<ProfileToken>%s</ProfileToken>"
             "</GetSnapshotUri>", profile_token);
    
    soap_message_t request;
    int ret = soap_create_envelope(&request,
                                   "http://www.onvif.org/ver10/media/wsdl",
                                   "http://www.onvif.org/ver10/media/wsdl/GetSnapshotUri",
                                   body);
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Send HTTP request */
    http_response_t response;
    char media_service_url[ONVIF_MAX_URL_LEN];
    snprintf(media_service_url, sizeof(media_service_url),
             "%s/onvif/media_service", client->device_url);
    
    ret = http_post(media_service_url, request.xml_data, request.xml_len,
                   client->username, client->password, &response, client->timeout_ms);
    
    soap_free_message(&request);
    
    if (ret != ONVIF_OK) {
        return ret;
    }

    /* Parse snapshot URI from response */
    soap_extract_value(response.body, "Uri", uri, max_len);
    
    http_free_response(&response);
    
    return ONVIF_OK;
}
