/**
 * @file server_example.c
 * @brief Example ONVIF Server Application
 * 
 * This demonstrates how to use the ONVIF server library.
 */

#include "../include/onvif_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static onvif_server_t server;
static volatile int running = 1;

void signal_handler(int signum) {
    printf("\nReceived signal %d, stopping server...\n", signum);
    running = 0;
    onvif_server_stop(&server);
}

int main(int argc, char* argv[]) {
    int port = 8080;
    
    /* Parse command line arguments */
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number: %s\n", argv[1]);
            return 1;
        }
    }
    
    printf("========================================\n");
    printf("ONVIF Server Example\n");
    printf("========================================\n");
    printf("Port: %d\n", port);
    printf("========================================\n\n");
    
    /* Initialize server */
    printf("Initializing ONVIF server...\n");
    if (onvif_server_init(&server, port) != ONVIF_SERVER_OK) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }
    
    /* Configure device information */
    onvif_server_set_device_info(&server,
                                 "Example Manufacturer",
                                 "Example IP Camera Model",
                                 "1.0.0",
                                 "EXAMPLE-SN-12345");
    
    /* Add users for authentication */
    onvif_server_set_auth_required(&server, true);
    onvif_server_add_user(&server, "admin", "admin123");
    onvif_server_add_user(&server, "operator", "oper123");
    printf("Added users: admin, operator\n");
    
    /* Add media profiles */
    onvif_media_profile_t profile1;
    memset(&profile1, 0, sizeof(profile1));
    strncpy(profile1.token, "profile_1", sizeof(profile1.token) - 1);
    strncpy(profile1.name, "MainStream", sizeof(profile1.name) - 1);
    strncpy(profile1.encoding, "H264", sizeof(profile1.encoding) - 1);
    profile1.width = 1920;
    profile1.height = 1080;
    profile1.frame_rate_limit = 30;
    profile1.quality = 5;
    profile1.bitrate_limit = 4000000;
    profile1.fixed = true;
    snprintf(profile1.rtsp_uri, sizeof(profile1.rtsp_uri),
             "rtsp://localhost:%d/stream1", port);
    snprintf(profile1.snapshot_uri, sizeof(profile1.snapshot_uri),
             "http://localhost:%d/snapshot1.jpg", port);
    
    onvif_server_add_profile(&server, &profile1);
    
    onvif_media_profile_t profile2;
    memset(&profile2, 0, sizeof(profile2));
    strncpy(profile2.token, "profile_2", sizeof(profile2.token) - 1);
    strncpy(profile2.name, "SubStream", sizeof(profile2.name) - 1);
    strncpy(profile2.encoding, "H264", sizeof(profile2.encoding) - 1);
    profile2.width = 640;
    profile2.height = 480;
    profile2.frame_rate_limit = 15;
    profile2.quality = 3;
    profile2.bitrate_limit = 1000000;
    profile2.fixed = false;
    snprintf(profile2.rtsp_uri, sizeof(profile2.rtsp_uri),
             "rtsp://localhost:%d/stream2", port);
    snprintf(profile2.snapshot_uri, sizeof(profile2.snapshot_uri),
             "http://localhost:%d/snapshot2.jpg", port);
    
    onvif_server_add_profile(&server, &profile2);
    
    printf("Added %d media profiles\n", server.profile_count);
    printf("\nDevice Service URL: %s\n", server.device_service.xaddr);
    printf("Media Service URL:  %s\n", server.media_service.xaddr);
    
    /* Setup signal handler */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("\nStarting server (Press Ctrl+C to stop)...\n");
    printf("========================================\n");
    
    /* Start server in a non-blocking way for demo */
    /* In production, onvif_server_start() would block */
    printf("\nServer is configured and ready.\n");
    printf("\nYou can now connect ONVIF clients to:\n");
    printf("  Device Service: %s\n", server.device_service.xaddr);
    printf("  Media Service:  %s\n", server.media_service.xaddr);
    printf("\nAuthentication credentials:\n");
    printf("  Username: admin    Password: admin123\n");
    printf("  Username: operator Password: oper123\n");
    printf("\nAvailable profiles:\n");
    for (int i = 0; i < server.profile_count; i++) {
        printf("  [%d] %s (%s) - %dx%d @ %d fps\n",
               i + 1,
               server.profiles[i].name,
               server.profiles[i].token,
               server.profiles[i].width,
               server.profiles[i].height,
               server.profiles[i].frame_rate_limit);
        printf("      Stream:   %s\n", server.profiles[i].rtsp_uri);
        printf("      Snapshot: %s\n", server.profiles[i].snapshot_uri);
    }
    
    printf("\n========================================\n");
    printf("Note: This is a demonstration server.\n");
    printf("For full functionality with gSOAP, rebuild with USE_GSOAP=ON\n");
    printf("========================================\n");
    
    /* Keep running until interrupted */
    while (running) {
        sleep(1);
    }
    
    /* Cleanup */
    printf("\nCleaning up...\n");
    onvif_server_destroy(&server);
    
    printf("Server stopped successfully.\n");
    return 0;
}
