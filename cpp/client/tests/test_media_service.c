#include "../include/onvif_client.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Simple test framework */
#define TEST(name) void name()
#define RUN_TEST(test) do { printf("Running %s...\n", #test); test(); printf("  PASSED\n"); } while(0)

TEST(test_get_profiles_null_params) {
    onvif_client_t client;
    onvif_media_profile_t profiles[ONVIF_MAX_PROFILES];
    int count = 0;
    
    onvif_client_init(&client, "http://192.168.1.100", "admin", "password");
    
    int ret = onvif_get_profiles(NULL, profiles, &count, ONVIF_MAX_PROFILES);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_profiles(&client, NULL, &count, ONVIF_MAX_PROFILES);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_profiles(&client, profiles, NULL, ONVIF_MAX_PROFILES);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_profiles(&client, profiles, &count, 0);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    onvif_client_destroy(&client);
}

TEST(test_get_stream_uri_null_params) {
    onvif_client_t client;
    onvif_stream_uri_t uri;
    
    onvif_client_init(&client, "http://192.168.1.100", "admin", "password");
    
    int ret = onvif_get_stream_uri(NULL, "Profile_1", &uri);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_stream_uri(&client, NULL, &uri);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_stream_uri(&client, "Profile_1", NULL);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    onvif_client_destroy(&client);
}

TEST(test_get_snapshot_uri_null_params) {
    onvif_client_t client;
    char uri[ONVIF_MAX_URL_LEN];
    
    onvif_client_init(&client, "http://192.168.1.100", "admin", "password");
    
    int ret = onvif_get_snapshot_uri(NULL, "Profile_1", uri, sizeof(uri));
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_snapshot_uri(&client, NULL, uri, sizeof(uri));
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_snapshot_uri(&client, "Profile_1", NULL, sizeof(uri));
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    ret = onvif_get_snapshot_uri(&client, "Profile_1", uri, 0);
    assert(ret == ONVIF_ERR_INVALID_PARAM);
    
    onvif_client_destroy(&client);
}

int main() {
    printf("=== ONVIF Media Service Tests ===\n");
    
    RUN_TEST(test_get_profiles_null_params);
    RUN_TEST(test_get_stream_uri_null_params);
    RUN_TEST(test_get_snapshot_uri_null_params);
    
    printf("\nAll tests passed!\n");
    return 0;
}
