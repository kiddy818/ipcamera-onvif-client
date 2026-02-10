#include "../include/onvif_client.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Simple test framework */
#define TEST(name) void name()
#define RUN_TEST(test) do { printf("Running %s...\n", #test); test(); printf("  PASSED\n"); } while(0)

TEST(test_get_device_info_null_params) {
    onvif_client_t client;
    onvif_device_info_t info;
    
    onvif_client_init(&client, "http://192.168.1.100", "admin", "password");
    
    int ret = onvif_get_device_information(NULL, &info);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_device_information(&client, NULL);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    onvif_client_destroy(&client);
}

TEST(test_get_capabilities_null_params) {
    onvif_client_t client;
    onvif_capabilities_t caps;
    
    onvif_client_init(&client, "http://192.168.1.100", "admin", "password");
    
    int ret = onvif_get_capabilities(NULL, &caps);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_capabilities(&client, NULL);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    onvif_client_destroy(&client);
}

TEST(test_get_services_null_params) {
    onvif_client_t client;
    char services[1024];
    
    onvif_client_init(&client, "http://192.168.1.100", "admin", "password");
    
    int ret = onvif_get_services(NULL, services, sizeof(services));
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_services(&client, NULL, sizeof(services));
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_services(&client, services, 0);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    onvif_client_destroy(&client);
}

int main() {
    printf("=== ONVIF Device Service Tests ===\n");
    
    RUN_TEST(test_get_device_info_null_params);
    RUN_TEST(test_get_capabilities_null_params);
    RUN_TEST(test_get_services_null_params);
    
    printf("\nAll tests passed!\n");
    return 0;
}
