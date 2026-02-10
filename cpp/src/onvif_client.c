#include "onvif_client.h"
#include <string.h>
#include <stdio.h>

int onvif_client_init(onvif_client_t* client, const char* device_url,
                      const char* username, const char* password) {
    if (!client || !device_url) {
        return ONVIF_ERR_INVALID_PARAM;
    }

    memset(client, 0, sizeof(onvif_client_t));
    
    strncpy(client->device_url, device_url, ONVIF_MAX_URL_LEN - 1);
    client->device_url[ONVIF_MAX_URL_LEN - 1] = '\0';
    
    if (username) {
        strncpy(client->username, username, ONVIF_MAX_STRING_LEN - 1);
        client->username[ONVIF_MAX_STRING_LEN - 1] = '\0';
    }
    
    if (password) {
        strncpy(client->password, password, ONVIF_MAX_STRING_LEN - 1);
        client->password[ONVIF_MAX_STRING_LEN - 1] = '\0';
    }
    
    client->timeout_ms = 5000; /* Default 5 seconds */
    
    return ONVIF_OK;
}

void onvif_client_destroy(onvif_client_t* client) {
    if (client) {
        memset(client, 0, sizeof(onvif_client_t));
    }
}
