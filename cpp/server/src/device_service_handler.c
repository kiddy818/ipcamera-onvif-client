/**
 * @file device_service_handler.c
 * @brief ONVIF Device Management Service Handler Implementation
 */

#include "../include/device_service_handler.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int handle_get_device_information(onvif_server_t* server,
                                  char* response_xml,
                                  size_t max_len) {
    if (!server || !response_xml || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    int written = snprintf(response_xml, max_len,
        "<tds:GetDeviceInformationResponse xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">"
        "<tds:Manufacturer>%s</tds:Manufacturer>"
        "<tds:Model>%s</tds:Model>"
        "<tds:FirmwareVersion>%s</tds:FirmwareVersion>"
        "<tds:SerialNumber>%s</tds:SerialNumber>"
        "<tds:HardwareId>%s</tds:HardwareId>"
        "</tds:GetDeviceInformationResponse>",
        server->device_info.manufacturer,
        server->device_info.model,
        server->device_info.firmware_version,
        server->device_info.serial_number,
        server->device_info.hardware_id[0] ? server->device_info.hardware_id : "N/A");
    
    if (written < 0 || (size_t)written >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    return ONVIF_SERVER_OK;
}

int handle_get_capabilities(onvif_server_t* server,
                            char* response_xml,
                            size_t max_len) {
    if (!server || !response_xml || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    int written = snprintf(response_xml, max_len,
        "<tds:GetCapabilitiesResponse xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\" "
        "xmlns:tt=\"http://www.onvif.org/ver10/schema\">"
        "<tds:Capabilities>"
        "<tt:Device>"
        "<tt:XAddr>%s</tt:XAddr>"
        "<tt:Network>"
        "<tt:IPFilter>false</tt:IPFilter>"
        "<tt:ZeroConfiguration>false</tt:ZeroConfiguration>"
        "<tt:IPVersion6>false</tt:IPVersion6>"
        "<tt:DynDNS>false</tt:DynDNS>"
        "</tt:Network>"
        "<tt:System>"
        "<tt:DiscoveryResolve>false</tt:DiscoveryResolve>"
        "<tt:DiscoveryBye>false</tt:DiscoveryBye>"
        "<tt:RemoteDiscovery>false</tt:RemoteDiscovery>"
        "<tt:SystemBackup>false</tt:SystemBackup>"
        "<tt:SystemLogging>false</tt:SystemLogging>"
        "<tt:FirmwareUpgrade>false</tt:FirmwareUpgrade>"
        "</tt:System>"
        "<tt:Security>"
        "<tt:TLS1.1>false</tt:TLS1.1>"
        "<tt:TLS1.2>false</tt:TLS1.2>"
        "<tt:OnboardKeyGeneration>false</tt:OnboardKeyGeneration>"
        "<tt:AccessPolicyConfig>false</tt:AccessPolicyConfig>"
        "<tt:X.509Token>false</tt:X.509Token>"
        "<tt:SAMLToken>false</tt:SAMLToken>"
        "<tt:KerberosToken>false</tt:KerberosToken>"
        "<tt:RELToken>false</tt:RELToken>"
        "</tt:Security>"
        "</tt:Device>"
        "<tt:Media>"
        "<tt:XAddr>%s</tt:XAddr>"
        "<tt:StreamingCapabilities>"
        "<tt:RTPMulticast>false</tt:RTPMulticast>"
        "<tt:RTP_TCP>true</tt:RTP_TCP>"
        "<tt:RTP_RTSP_TCP>true</tt:RTP_RTSP_TCP>"
        "</tt:StreamingCapabilities>"
        "</tt:Media>",
        server->device_service.xaddr,
        server->media_service.xaddr);
    
    if (server->ptz_enabled) {
        int ptz_written = snprintf(response_xml + written, max_len - written,
            "<tt:PTZ>"
            "<tt:XAddr>%s</tt:XAddr>"
            "</tt:PTZ>",
            server->ptz_service.xaddr);
        
        if (ptz_written < 0 || (size_t)(written + ptz_written) >= max_len) {
            return ONVIF_SERVER_ERR_MEMORY;
        }
        written += ptz_written;
    }
    
    int end_written = snprintf(response_xml + written, max_len - written,
        "</tds:Capabilities>"
        "</tds:GetCapabilitiesResponse>");
    
    if (end_written < 0 || (size_t)(written + end_written) >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    return ONVIF_SERVER_OK;
}

int handle_get_services(onvif_server_t* server,
                       char* response_xml,
                       size_t max_len) {
    if (!server || !response_xml || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    int written = snprintf(response_xml, max_len,
        "<tds:GetServicesResponse xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">"
        "<tds:Service>"
        "<tds:Namespace>%s</tds:Namespace>"
        "<tds:XAddr>%s</tds:XAddr>"
        "<tds:Version>"
        "<tds:Major>%d</tds:Major>"
        "<tds:Minor>%d</tds:Minor>"
        "</tds:Version>"
        "</tds:Service>"
        "<tds:Service>"
        "<tds:Namespace>%s</tds:Namespace>"
        "<tds:XAddr>%s</tds:XAddr>"
        "<tds:Version>"
        "<tds:Major>%d</tds:Major>"
        "<tds:Minor>%d</tds:Minor>"
        "</tds:Version>"
        "</tds:Service>",
        server->device_service.namespace_uri,
        server->device_service.xaddr,
        server->device_service.version_major,
        server->device_service.version_minor,
        server->media_service.namespace_uri,
        server->media_service.xaddr,
        server->media_service.version_major,
        server->media_service.version_minor);
    
    if (written < 0 || (size_t)written >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    if (server->ptz_enabled) {
        int ptz_written = snprintf(response_xml + written, max_len - written,
            "<tds:Service>"
            "<tds:Namespace>%s</tds:Namespace>"
            "<tds:XAddr>%s</tds:XAddr>"
            "<tds:Version>"
            "<tds:Major>%d</tds:Major>"
            "<tds:Minor>%d</tds:Minor>"
            "</tds:Version>"
            "</tds:Service>",
            server->ptz_service.namespace_uri,
            server->ptz_service.xaddr,
            server->ptz_service.version_major,
            server->ptz_service.version_minor);
        
        if (ptz_written < 0 || (size_t)(written + ptz_written) >= max_len) {
            return ONVIF_SERVER_ERR_MEMORY;
        }
        written += ptz_written;
    }
    
    int end_written = snprintf(response_xml + written, max_len - written,
        "</tds:GetServicesResponse>");
    
    if (end_written < 0 || (size_t)(written + end_written) >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    return ONVIF_SERVER_OK;
}

int handle_get_system_date_time(onvif_server_t* server,
                                char* response_xml,
                                size_t max_len) {
    if (!server || !response_xml || max_len == 0) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    time_t now = time(NULL);
    struct tm* utc_time = gmtime(&now);
    
    int written = snprintf(response_xml, max_len,
        "<tds:GetSystemDateAndTimeResponse xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\" "
        "xmlns:tt=\"http://www.onvif.org/ver10/schema\">"
        "<tds:SystemDateAndTime>"
        "<tt:DateTimeType>NTP</tt:DateTimeType>"
        "<tt:DaylightSavings>false</tt:DaylightSavings>"
        "<tt:TimeZone>"
        "<tt:TZ>UTC</tt:TZ>"
        "</tt:TimeZone>"
        "<tt:UTCDateTime>"
        "<tt:Time>"
        "<tt:Hour>%d</tt:Hour>"
        "<tt:Minute>%d</tt:Minute>"
        "<tt:Second>%d</tt:Second>"
        "</tt:Time>"
        "<tt:Date>"
        "<tt:Year>%d</tt:Year>"
        "<tt:Month>%d</tt:Month>"
        "<tt:Day>%d</tt:Day>"
        "</tt:Date>"
        "</tt:UTCDateTime>"
        "</tds:SystemDateAndTime>"
        "</tds:GetSystemDateAndTimeResponse>",
        utc_time->tm_hour,
        utc_time->tm_min,
        utc_time->tm_sec,
        utc_time->tm_year + 1900,
        utc_time->tm_mon + 1,
        utc_time->tm_mday);
    
    if (written < 0 || (size_t)written >= max_len) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    return ONVIF_SERVER_OK;
}
