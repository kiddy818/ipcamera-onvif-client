/**
 * @file media_service_handler.c
 * @brief ONVIF Media Service Handler Implementation
 */

#include "../include/media_service_handler.h"
#include <stdio.h>
#include <string.h>

int handle_get_profiles(onvif_server_t* server,
                       char* response_xml,
                       size_t max_len) {
    if (!server || !response_xml || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    int written = snprintf(response_xml, max_len,
        "<trt:GetProfilesResponse xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\" "
        "xmlns:tt=\"http://www.onvif.org/ver10/schema\">");
    
    if (written < 0 || (size_t)written >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    /* Add each profile */
    for (int i = 0; i < server->profile_count; i++) {
        onvif_media_profile_t* profile = &server->profiles[i];
        
        int profile_written = snprintf(response_xml + written, max_len - written,
            "<trt:Profiles token=\"%s\" fixed=\"%s\">"
            "<tt:Name>%s</tt:Name>"
            "<tt:VideoEncoderConfiguration>"
            "<tt:Encoding>%s</tt:Encoding>"
            "<tt:Resolution>"
            "<tt:Width>%d</tt:Width>"
            "<tt:Height>%d</tt:Height>"
            "</tt:Resolution>"
            "<tt:Quality>%d</tt:Quality>"
            "<tt:RateControl>"
            "<tt:FrameRateLimit>%d</tt:FrameRateLimit>"
            "<tt:BitrateLimit>%d</tt:BitrateLimit>"
            "</tt:RateControl>"
            "</tt:VideoEncoderConfiguration>"
            "</trt:Profiles>",
            profile->token,
            profile->fixed ? "true" : "false",
            profile->name,
            profile->encoding,
            profile->width,
            profile->height,
            profile->quality,
            profile->frame_rate_limit,
            profile->bitrate_limit);
        
        if (profile_written < 0 || (size_t)(written + profile_written) >= max_len) {
            return ONVIF_SERVER_ERR_MEMORY;
        }
        written += profile_written;
    }
    
    int end_written = snprintf(response_xml + written, max_len - written,
        "</trt:GetProfilesResponse>");
    
    if (end_written < 0 || (size_t)(written + end_written) >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    return ONVIF_SERVER_OK;
}

int handle_get_stream_uri(onvif_server_t* server,
                         const char* profile_token,
                         char* response_xml,
                         size_t max_len) {
    if (!server || !profile_token || !response_xml || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    /* Find profile by token */
    onvif_media_profile_t* profile = NULL;
    for (int i = 0; i < server->profile_count; i++) {
        if (strcmp(server->profiles[i].token, profile_token) == 0) {
            profile = &server->profiles[i];
            break;
        }
    }
    
    if (!profile) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    int written = snprintf(response_xml, max_len,
        "<trt:GetStreamUriResponse xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\" "
        "xmlns:tt=\"http://www.onvif.org/ver10/schema\">"
        "<trt:MediaUri>"
        "<tt:Uri>%s</tt:Uri>"
        "<tt:InvalidAfterConnect>false</tt:InvalidAfterConnect>"
        "<tt:InvalidAfterReboot>false</tt:InvalidAfterReboot>"
        "<tt:Timeout>PT60S</tt:Timeout>"
        "</trt:MediaUri>"
        "</trt:GetStreamUriResponse>",
        profile->rtsp_uri);
    
    if (written < 0 || (size_t)written >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    return ONVIF_SERVER_OK;
}

int handle_get_snapshot_uri(onvif_server_t* server,
                           const char* profile_token,
                           char* response_xml,
                           size_t max_len) {
    if (!server || !profile_token || !response_xml || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    /* Find profile by token */
    onvif_media_profile_t* profile = NULL;
    for (int i = 0; i < server->profile_count; i++) {
        if (strcmp(server->profiles[i].token, profile_token) == 0) {
            profile = &server->profiles[i];
            break;
        }
    }
    
    if (!profile) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    int written = snprintf(response_xml, max_len,
        "<trt:GetSnapshotUriResponse xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\" "
        "xmlns:tt=\"http://www.onvif.org/ver10/schema\">"
        "<trt:MediaUri>"
        "<tt:Uri>%s</tt:Uri>"
        "<tt:InvalidAfterConnect>false</tt:InvalidAfterConnect>"
        "<tt:InvalidAfterReboot>false</tt:InvalidAfterReboot>"
        "<tt:Timeout>PT0S</tt:Timeout>"
        "</trt:MediaUri>"
        "</trt:GetSnapshotUriResponse>",
        profile->snapshot_uri);
    
    if (written < 0 || (size_t)written >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    return ONVIF_SERVER_OK;
}

int handle_get_video_encoder_configuration(onvif_server_t* server,
                                          const char* config_token,
                                          char* response_xml,
                                          size_t max_len) {
    if (!server || !config_token || !response_xml || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    /* For simplicity, use first profile's configuration */
    if (server->profile_count == 0) {
        return ONVIF_SERVER_ERR_NOT_SUPPORTED;
    }
    
    onvif_media_profile_t* profile = &server->profiles[0];
    
    int written = snprintf(response_xml, max_len,
        "<trt:GetVideoEncoderConfigurationResponse "
        "xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\" "
        "xmlns:tt=\"http://www.onvif.org/ver10/schema\">"
        "<trt:Configuration token=\"%s\">"
        "<tt:Name>VideoEncoderConfig</tt:Name>"
        "<tt:UseCount>1</tt:UseCount>"
        "<tt:Encoding>%s</tt:Encoding>"
        "<tt:Resolution>"
        "<tt:Width>%d</tt:Width>"
        "<tt:Height>%d</tt:Height>"
        "</tt:Resolution>"
        "<tt:Quality>%d</tt:Quality>"
        "<tt:RateControl>"
        "<tt:FrameRateLimit>%d</tt:FrameRateLimit>"
        "<tt:EncodingInterval>1</tt:EncodingInterval>"
        "<tt:BitrateLimit>%d</tt:BitrateLimit>"
        "</tt:RateControl>"
        "</trt:Configuration>"
        "</trt:GetVideoEncoderConfigurationResponse>",
        config_token,
        profile->encoding,
        profile->width,
        profile->height,
        profile->quality,
        profile->frame_rate_limit,
        profile->bitrate_limit);
    
    if (written < 0 || (size_t)written >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    return ONVIF_SERVER_OK;
}
