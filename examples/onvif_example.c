#include "onvif_client.h"
#include <stdio.h>
#include <string.h>

void print_device_info(onvif_device_info_t* info) {
    printf("Device Information:\n");
    printf("  Manufacturer: %s\n", info->manufacturer);
    printf("  Model: %s\n", info->model);
    printf("  Firmware Version: %s\n", info->firmware_version);
    printf("  Serial Number: %s\n", info->serial_number);
    printf("  Hardware ID: %s\n", info->hardware_id);
}

void print_capabilities(onvif_capabilities_t* caps) {
    printf("Device Capabilities:\n");
    printf("  Device Service: %s\n", caps->device_service ? "Yes" : "No");
    printf("  Media Service: %s\n", caps->media_service ? "Yes" : "No");
    printf("  PTZ Service: %s\n", caps->ptz_service ? "Yes" : "No");
    printf("  Imaging Service: %s\n", caps->imaging_service ? "Yes" : "No");
    printf("  Analytics Service: %s\n", caps->analytics_service ? "Yes" : "No");
}

void print_profiles(onvif_media_profile_t* profiles, int count) {
    printf("Media Profiles (%d):\n", count);
    for (int i = 0; i < count; i++) {
        printf("  Profile %d:\n", i + 1);
        printf("    Token: %s\n", profiles[i].token);
        printf("    Name: %s\n", profiles[i].name);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage: %s <device_url> <username> <password>\n", argv[0]);
        printf("Example: %s http://192.168.1.100 admin password123\n", argv[0]);
        return 1;
    }

    const char* device_url = argv[1];
    const char* username = argv[2];
    const char* password = argv[3];

    printf("=== ONVIF Client Example ===\n\n");

    /* Initialize client */
    onvif_client_t client;
    int ret = onvif_client_init(&client, device_url, username, password);
    if (ret != ONVIF_OK) {
        printf("Error: Failed to initialize ONVIF client (error: %d)\n", ret);
        return 1;
    }
    printf("✓ Client initialized\n\n");

    /* Get device information */
    printf("--- Getting Device Information ---\n");
    onvif_device_info_t info;
    ret = onvif_get_device_information(&client, &info);
    if (ret == ONVIF_OK) {
        print_device_info(&info);
    } else {
        printf("Error: Failed to get device information (error: %d)\n", ret);
        printf("Note: This requires a real ONVIF device or mock server\n");
    }
    printf("\n");

    /* Get capabilities */
    printf("--- Getting Device Capabilities ---\n");
    onvif_capabilities_t caps;
    ret = onvif_get_capabilities(&client, &caps);
    if (ret == ONVIF_OK) {
        print_capabilities(&caps);
    } else {
        printf("Error: Failed to get capabilities (error: %d)\n", ret);
        printf("Note: This requires a real ONVIF device or mock server\n");
    }
    printf("\n");

    /* Get media profiles */
    printf("--- Getting Media Profiles ---\n");
    onvif_media_profile_t profiles[ONVIF_MAX_PROFILES];
    int profile_count = 0;
    ret = onvif_get_profiles(&client, profiles, &profile_count, ONVIF_MAX_PROFILES);
    if (ret == ONVIF_OK) {
        print_profiles(profiles, profile_count);
        
        /* Get stream URI for first profile */
        if (profile_count > 0) {
            printf("\n--- Getting Stream URI for Profile: %s ---\n", profiles[0].token);
            onvif_stream_uri_t stream_uri;
            ret = onvif_get_stream_uri(&client, profiles[0].token, &stream_uri);
            if (ret == ONVIF_OK) {
                printf("Stream URI: %s\n", stream_uri.uri);
                printf("Timeout: %d seconds\n", stream_uri.timeout_sec);
            } else {
                printf("Error: Failed to get stream URI (error: %d)\n", ret);
            }
        }
    } else {
        printf("Error: Failed to get profiles (error: %d)\n", ret);
        printf("Note: This requires a real ONVIF device or mock server\n");
    }
    printf("\n");

    /* Cleanup */
    onvif_client_destroy(&client);
    printf("✓ Client destroyed\n");

    return 0;
}
