#include "../include/onvif_client.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Simple test framework */
#define TEST(name) void name()
#define RUN_TEST(test) do { printf("Running %s...\n", #test); test(); printf("  PASSED\n"); } while(0)

TEST(test_client_init) {
    onvif_client_t client;
    int ret = onvif_client_init(&client, "http://192.168.1.100", "admin", "password");
    assert(ret == ONVIF_OK);
    assert(strcmp(client.device_url, "http://192.168.1.100") == 0);
    assert(strcmp(client.username, "admin") == 0);
    assert(strcmp(client.password, "password") == 0);
    assert(client.timeout_ms == 5000);
    onvif_client_destroy(&client);
}

TEST(test_client_init_null_params) {
    onvif_client_t client;
    int ret = onvif_client_init(NULL, "http://192.168.1.100", "admin", "password");
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_client_init(&client, NULL, "admin", "password");
    assert(ret == ONVIF_ERR_INVALID_PARAM);
}

TEST(test_client_init_no_auth) {
    onvif_client_t client;
    int ret = onvif_client_init(&client, "http://192.168.1.100", NULL, NULL);
    assert(ret == ONVIF_OK);
    assert(strcmp(client.device_url, "http://192.168.1.100") == 0);
    assert(client.username[0] == '\0');
    assert(client.password[0] == '\0');
    onvif_client_destroy(&client);
}

int main() {
    printf("=== ONVIF Client Tests ===\n");
    
    RUN_TEST(test_client_init);
    RUN_TEST(test_client_init_null_params);
    RUN_TEST(test_client_init_no_auth);
    
    printf("\nAll tests passed!\n");
    return 0;
}
