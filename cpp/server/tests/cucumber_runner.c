/**
 * @file cucumber_runner.c
 * @brief BDD Test Runner for ONVIF Server
 * 
 * This implements BDD-style testing for the ONVIF server.
 * Tests are based on the feature files in the features/ directory.
 */

#include "../include/onvif_server.h"
#include "../include/device_service_handler.h"
#include "../include/media_service_handler.h"
#include "../include/soap_server_handler.h"
#include "../include/auth_handler.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Test state */
static onvif_server_t test_server;
static bool server_initialized = false;

/* Find an available port for testing */
static int find_available_port() {
    /* Use a range of ports for testing to avoid conflicts */
    static int test_port = 18080;
    return test_port++;
}

/* Test helper functions */
static void setup_test_server() {
    int port = find_available_port();
    onvif_server_init(&test_server, port);
    onvif_server_set_device_info(&test_server,
                                 "TestManufacturer",
                                 "TestModel",
                                 "1.0.0",
                                 "SN123456");
    server_initialized = true;
}

static void teardown_test_server() {
    if (server_initialized) {
        onvif_server_destroy(&test_server);
        server_initialized = false;
    }
}

/* Feature: Server Device Management Service */

int test_server_initialization() {
    printf("\n  Scenario: Server initialization\n");
    printf("    Given the ONVIF server is configured on a test port\n");
    
    onvif_server_t server;
    int port = find_available_port();
    int ret = onvif_server_init(&server, port);
    
    printf("    When I start the ONVIF server initialization\n");
    printf("    Then the server should be initialized successfully\n");
    
    if (ret == ONVIF_SERVER_OK) {
        printf("    ✓ PASSED (port: %d)\n", port);
        onvif_server_destroy(&server);
        return 0;
    }
    
    printf("    ✗ FAILED: initialization returned %d\n", ret);
    return -1;
}

int test_handle_get_device_information() {
    printf("\n  Scenario: Handle GetDeviceInformation request\n");
    printf("    Given the ONVIF server is running\n");
    
    setup_test_server();
    
    printf("    When a client requests device information\n");
    
    char response[4096];
    int ret = handle_get_device_information(&test_server, response, sizeof(response));
    
    printf("    Then the server should return device information\n");
    printf("    And the response should be valid SOAP format\n");
    
    if (ret == ONVIF_SERVER_OK && strlen(response) > 0) {
        /* Check for required fields */
        if (strstr(response, "TestManufacturer") &&
            strstr(response, "TestModel") &&
            strstr(response, "1.0.0")) {
            printf("    ✓ PASSED\n");
            teardown_test_server();
            return 0;
        }
    }
    
    printf("    ✗ FAILED\n");
    teardown_test_server();
    return -1;
}

int test_handle_get_capabilities() {
    printf("\n  Scenario: Handle GetCapabilities request\n");
    printf("    Given the ONVIF server is running\n");
    
    setup_test_server();
    
    printf("    When a client requests device capabilities\n");
    
    char response[4096];
    int ret = handle_get_capabilities(&test_server, response, sizeof(response));
    
    printf("    Then the server should return capabilities\n");
    
    if (ret == ONVIF_SERVER_OK && strlen(response) > 0) {
        if (strstr(response, "Device") && strstr(response, "Media")) {
            printf("    ✓ PASSED\n");
            teardown_test_server();
            return 0;
        }
    }
    
    printf("    ✗ FAILED\n");
    teardown_test_server();
    return -1;
}

int test_handle_get_services() {
    printf("\n  Scenario: Handle GetServices request\n");
    printf("    Given the ONVIF server is running\n");
    
    setup_test_server();
    
    printf("    When a client requests available services\n");
    
    char response[4096];
    int ret = handle_get_services(&test_server, response, sizeof(response));
    
    printf("    Then the server should return a list of services\n");
    
    if (ret == ONVIF_SERVER_OK && strlen(response) > 0) {
        if (strstr(response, "device/wsdl") && strstr(response, "media/wsdl")) {
            printf("    ✓ PASSED\n");
            teardown_test_server();
            return 0;
        }
    }
    
    printf("    ✗ FAILED\n");
    teardown_test_server();
    return -1;
}

int test_handle_get_system_date_time() {
    printf("\n  Scenario: Handle GetSystemDateAndTime request\n");
    printf("    Given the ONVIF server is running\n");
    
    setup_test_server();
    
    printf("    When a client requests system date and time\n");
    
    char response[4096];
    int ret = handle_get_system_date_time(&test_server, response, sizeof(response));
    
    printf("    Then the server should return current date and time\n");
    
    if (ret == ONVIF_SERVER_OK && strlen(response) > 0) {
        if (strstr(response, "UTCDateTime")) {
            printf("    ✓ PASSED\n");
            teardown_test_server();
            return 0;
        }
    }
    
    printf("    ✗ FAILED\n");
    teardown_test_server();
    return -1;
}

/* Feature: Server Media Service */

int test_handle_get_profiles() {
    printf("\n  Scenario: Handle GetProfiles request\n");
    printf("    Given the ONVIF server is running\n");
    
    setup_test_server();
    
    /* Add a test profile */
    onvif_media_profile_t profile;
    memset(&profile, 0, sizeof(profile));
    strncpy(profile.token, "profile_1", sizeof(profile.token) - 1);
    strncpy(profile.name, "MainProfile", sizeof(profile.name) - 1);
    strncpy(profile.encoding, "H264", sizeof(profile.encoding) - 1);
    profile.width = 1920;
    profile.height = 1080;
    profile.frame_rate_limit = 30;
    profile.quality = 5;
    profile.bitrate_limit = 4000000;
    strncpy(profile.rtsp_uri, "rtsp://192.168.1.100:554/stream1", 
            sizeof(profile.rtsp_uri) - 1);
    
    onvif_server_add_profile(&test_server, &profile);
    
    printf("    And media profiles are configured\n");
    printf("    When a client requests media profiles\n");
    
    char response[4096];
    int ret = handle_get_profiles(&test_server, response, sizeof(response));
    
    printf("    Then the server should return media profiles\n");
    
    if (ret == ONVIF_SERVER_OK && strlen(response) > 0) {
        if (strstr(response, "profile_1") && strstr(response, "MainProfile")) {
            printf("    ✓ PASSED\n");
            teardown_test_server();
            return 0;
        }
    }
    
    printf("    ✗ FAILED\n");
    teardown_test_server();
    return -1;
}

int test_handle_get_stream_uri() {
    printf("\n  Scenario: Handle GetStreamUri request\n");
    printf("    Given the ONVIF server is running\n");
    
    setup_test_server();
    
    /* Add a test profile */
    onvif_media_profile_t profile;
    memset(&profile, 0, sizeof(profile));
    strncpy(profile.token, "profile_1", sizeof(profile.token) - 1);
    strncpy(profile.name, "MainProfile", sizeof(profile.name) - 1);
    strncpy(profile.rtsp_uri, "rtsp://192.168.1.100:554/stream1",
            sizeof(profile.rtsp_uri) - 1);
    
    onvif_server_add_profile(&test_server, &profile);
    
    printf("    And a media profile exists\n");
    printf("    When a client requests stream URI\n");
    
    char response[4096];
    int ret = handle_get_stream_uri(&test_server, "profile_1", 
                                    response, sizeof(response));
    
    printf("    Then the server should return RTSP stream URI\n");
    
    if (ret == ONVIF_SERVER_OK && strlen(response) > 0) {
        if (strstr(response, "rtsp://")) {
            printf("    ✓ PASSED\n");
            teardown_test_server();
            return 0;
        }
    }
    
    printf("    ✗ FAILED\n");
    teardown_test_server();
    return -1;
}

/* Feature: SOAP Handling */

int test_soap_parse_request() {
    printf("\n  Scenario: Parse valid SOAP request\n");
    printf("    Given a valid SOAP request\n");
    
    const char* test_request = 
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">"
        "<s:Body>"
        "<tds:GetDeviceInformation xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\"/>"
        "</s:Body>"
        "</s:Envelope>";
    
    printf("    When parsing the SOAP request\n");
    
    char action[256];
    char body[4096];
    int ret = soap_parse_request(test_request, action, sizeof(action),
                                 body, sizeof(body));
    
    printf("    Then the server should extract the action\n");
    
    if (ret == ONVIF_SERVER_OK && strlen(action) > 0) {
        if (strstr(action, "GetDeviceInformation")) {
            printf("    ✓ PASSED (action: %s)\n", action);
            return 0;
        }
    }
    
    printf("    ✗ FAILED\n");
    return -1;
}

int test_soap_create_response() {
    printf("\n  Scenario: Create SOAP response\n");
    printf("    Given response body content\n");
    
    const char* body = "<Response>Test</Response>";
    
    printf("    When creating SOAP response\n");
    
    char response[4096];
    int ret = soap_create_response(body, response, sizeof(response));
    
    printf("    Then the server should create valid SOAP envelope\n");
    
    if (ret == ONVIF_SERVER_OK && strlen(response) > 0) {
        if (strstr(response, "Envelope") && strstr(response, body)) {
            printf("    ✓ PASSED\n");
            return 0;
        }
    }
    
    printf("    ✗ FAILED\n");
    return -1;
}

int test_soap_create_fault() {
    printf("\n  Scenario: Create SOAP fault\n");
    printf("    Given an error condition\n");
    
    printf("    When creating SOAP fault\n");
    
    char response[4096];
    int ret = soap_create_fault("s:Sender", "Authentication Failed",
                               response, sizeof(response));
    
    printf("    Then the server should create valid SOAP fault\n");
    
    if (ret == ONVIF_SERVER_OK && strlen(response) > 0) {
        if (strstr(response, "Fault") && strstr(response, "Authentication Failed")) {
            printf("    ✓ PASSED\n");
            return 0;
        }
    }
    
    printf("    ✗ FAILED\n");
    return -1;
}

/* Feature: Authentication */

int test_authentication_add_user() {
    printf("\n  Scenario: Add user for authentication\n");
    printf("    Given the ONVIF server is configured\n");
    
    setup_test_server();
    onvif_server_set_auth_required(&test_server, true);
    
    printf("    When adding a user\n");
    
    int ret = onvif_server_add_user(&test_server, "admin", "password123");
    
    printf("    Then the user should be added successfully\n");
    
    if (ret == ONVIF_SERVER_OK && test_server.user_count == 1) {
        printf("    ✓ PASSED\n");
        teardown_test_server();
        return 0;
    }
    
    printf("    ✗ FAILED\n");
    teardown_test_server();
    return -1;
}

/* Test suite runner */
typedef struct {
    const char* feature;
    const char* scenario;
    int (*test_func)();
} bdd_test_t;

static bdd_test_t tests[] = {
    /* Device Management Service */
    {"Server Device Management Service", "Server initialization", test_server_initialization},
    {"Server Device Management Service", "Handle GetDeviceInformation", test_handle_get_device_information},
    {"Server Device Management Service", "Handle GetCapabilities", test_handle_get_capabilities},
    {"Server Device Management Service", "Handle GetServices", test_handle_get_services},
    {"Server Device Management Service", "Handle GetSystemDateAndTime", test_handle_get_system_date_time},
    
    /* Media Service */
    {"Server Media Service", "Handle GetProfiles", test_handle_get_profiles},
    {"Server Media Service", "Handle GetStreamUri", test_handle_get_stream_uri},
    
    /* SOAP Handling */
    {"SOAP Message Handling", "Parse SOAP request", test_soap_parse_request},
    {"SOAP Message Handling", "Create SOAP response", test_soap_create_response},
    {"SOAP Message Handling", "Create SOAP fault", test_soap_create_fault},
    
    /* Authentication */
    {"Authentication", "Add user", test_authentication_add_user},
};

int main() {
    printf("========================================\n");
    printf("ONVIF Server BDD Test Suite\n");
    printf("========================================\n");
    
    int total = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;
    int failed = 0;
    
    const char* current_feature = NULL;
    
    for (int i = 0; i < total; i++) {
        if (!current_feature || strcmp(current_feature, tests[i].feature) != 0) {
            current_feature = tests[i].feature;
            printf("\nFeature: %s\n", current_feature);
        }
        
        int result = tests[i].test_func();
        if (result == 0) {
            passed++;
        } else {
            failed++;
        }
    }
    
    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Total:  %d\n", total);
    printf("  Passed: %d\n", passed);
    printf("  Failed: %d\n", failed);
    printf("========================================\n");
    
    return (failed == 0) ? 0 : 1;
}
