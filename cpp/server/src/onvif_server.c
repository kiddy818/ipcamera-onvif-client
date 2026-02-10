/**
 * @file onvif_server.c
 * @brief ONVIF Profile S Server Implementation
 */

#include "../include/onvif_server.h"
#include "../include/auth_handler.h"
#include "../include/soap_server_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int onvif_server_init(onvif_server_t* server, int port) {
    if (!server || port <= 0 || port > 65535) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    memset(server, 0, sizeof(onvif_server_t));
    
    /* Network settings */
    server->port = port;
    strncpy(server->bind_address, "0.0.0.0", sizeof(server->bind_address) - 1);
    server->max_connections = 10;
    server->timeout_sec = 30;
    
    /* Default device information */
    strncpy(server->device_info.manufacturer, "ONVIF Device", 
            sizeof(server->device_info.manufacturer) - 1);
    strncpy(server->device_info.model, "ONVIF Server v1.0", 
            sizeof(server->device_info.model) - 1);
    strncpy(server->device_info.firmware_version, "1.0.0", 
            sizeof(server->device_info.firmware_version) - 1);
    strncpy(server->device_info.serial_number, "000000000001", 
            sizeof(server->device_info.serial_number) - 1);
    
    /* Authentication disabled by default */
    server->require_auth = false;
    server->user_count = 0;
    
    /* Initialize service endpoints */
    char base_url[ONVIF_MAX_URL_LEN];
    snprintf(base_url, sizeof(base_url), "http://%s:%d", 
             server->bind_address, server->port);
    
    snprintf(server->device_service.xaddr, sizeof(server->device_service.xaddr),
             "%s/onvif/device_service", base_url);
    strncpy(server->device_service.namespace, 
            "http://www.onvif.org/ver10/device/wsdl",
            sizeof(server->device_service.namespace) - 1);
    server->device_service.version_major = 2;
    server->device_service.version_minor = 0;
    
    snprintf(server->media_service.xaddr, sizeof(server->media_service.xaddr),
             "%s/onvif/media_service", base_url);
    strncpy(server->media_service.namespace,
            "http://www.onvif.org/ver10/media/wsdl",
            sizeof(server->media_service.namespace) - 1);
    server->media_service.version_major = 2;
    server->media_service.version_minor = 0;
    
    server->ptz_enabled = false;
    server->profile_count = 0;
    server->running = false;
    server->soap_context = NULL;
    
    return ONVIF_SERVER_OK;
}

int onvif_server_set_device_info(onvif_server_t* server,
                                  const char* manufacturer,
                                  const char* model,
                                  const char* firmware_version,
                                  const char* serial_number) {
    if (!server) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    if (manufacturer) {
        strncpy(server->device_info.manufacturer, manufacturer,
                sizeof(server->device_info.manufacturer) - 1);
    }
    
    if (model) {
        strncpy(server->device_info.model, model,
                sizeof(server->device_info.model) - 1);
    }
    
    if (firmware_version) {
        strncpy(server->device_info.firmware_version, firmware_version,
                sizeof(server->device_info.firmware_version) - 1);
    }
    
    if (serial_number) {
        strncpy(server->device_info.serial_number, serial_number,
                sizeof(server->device_info.serial_number) - 1);
    }
    
    return ONVIF_SERVER_OK;
}

int onvif_server_add_user(onvif_server_t* server,
                          const char* username,
                          const char* password) {
    if (!server || !username || !password) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    if (server->user_count >= ONVIF_MAX_USERS) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    onvif_user_t* user = &server->users[server->user_count];
    strncpy(user->username, username, sizeof(user->username) - 1);
    strncpy(user->password, password, sizeof(user->password) - 1);
    user->enabled = true;
    
    server->user_count++;
    
    return ONVIF_SERVER_OK;
}

int onvif_server_add_profile(onvif_server_t* server,
                             const onvif_media_profile_t* profile) {
    if (!server || !profile) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    if (server->profile_count >= ONVIF_MAX_PROFILES) {
        return ONVIF_SERVER_ERR_MEMORY;
    }
    
    memcpy(&server->profiles[server->profile_count], profile,
           sizeof(onvif_media_profile_t));
    server->profile_count++;
    
    return ONVIF_SERVER_OK;
}

void onvif_server_set_auth_required(onvif_server_t* server, bool require) {
    if (server) {
        server->require_auth = require;
    }
}

int onvif_server_start(onvif_server_t* server) {
    if (!server) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    if (server->running) {
        return ONVIF_SERVER_ERR_ALREADY_RUNNING;
    }
    
    /* Create socket */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        return ONVIF_SERVER_ERR_NETWORK;
    }
    
    /* Set socket options */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(server_fd);
        return ONVIF_SERVER_ERR_NETWORK;
    }
    
    /* Bind socket */
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server->port);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(server_fd);
        return ONVIF_SERVER_ERR_NETWORK;
    }
    
    /* Listen for connections */
    if (listen(server_fd, server->max_connections) < 0) {
        close(server_fd);
        return ONVIF_SERVER_ERR_NETWORK;
    }
    
    server->running = true;
    printf("ONVIF Server started on port %d\n", server->port);
    
    /* Main server loop */
    while (server->running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (server->running) {
                fprintf(stderr, "Accept failed\n");
            }
            continue;
        }
        
        /* Handle client request in a simple way for now */
        /* In production, this should be handled in a separate thread/process */
        char buffer[ONVIF_MAX_REQUEST_SIZE];
        ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            
            /* Check if request was truncated */
            if (bytes_read >= (ssize_t)(sizeof(buffer) - 1)) {
                /* Request too large - return HTTP 413 */
                const char* error_response = 
                    "HTTP/1.1 413 Payload Too Large\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "Request too large";
                send(client_fd, error_response, strlen(error_response), 0);
                close(client_fd);
                continue;
            }
            
            /* Simple HTTP request parsing */
            char response[4096];
            snprintf(response, sizeof(response),
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: application/soap+xml\r\n"
                    "Content-Length: %zu\r\n"
                    "\r\n"
                    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                    "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">"
                    "<s:Body><Response>Server Running</Response></s:Body>"
                    "</s:Envelope>",
                    strlen("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                          "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">"
                          "<s:Body><Response>Server Running</Response></s:Body>"
                          "</s:Envelope>"));
            
            send(client_fd, response, strlen(response), 0);
        }
        
        close(client_fd);
    }
    
    close(server_fd);
    return ONVIF_SERVER_OK;
}

int onvif_server_stop(onvif_server_t* server) {
    if (!server) {
        return ONVIF_SERVER_ERR_INVALID_PARAM;
    }
    
    server->running = false;
    printf("ONVIF Server stopped\n");
    
    return ONVIF_SERVER_OK;
}

void onvif_server_destroy(onvif_server_t* server) {
    if (!server) {
        return;
    }
    
    if (server->running) {
        onvif_server_stop(server);
    }
    
    memset(server, 0, sizeof(onvif_server_t));
}

bool onvif_server_is_running(const onvif_server_t* server) {
    return server && server->running;
}
