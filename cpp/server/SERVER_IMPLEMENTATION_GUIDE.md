# ONVIF Profile S Server Implementation Guide

## Overview

This document describes the ONVIF Profile S server implementation using BDD (Behavior-Driven Development) approach with gSOAP library support.

## What is Implemented

The ONVIF server implements the following services compliant with ONVIF Profile S:

### Device Management Service
- **GetDeviceInformation**: Returns manufacturer, model, firmware version, and serial number
- **GetCapabilities**: Returns device capabilities including available services
- **GetServices**: Returns list of available ONVIF services with their endpoints
- **GetSystemDateAndTime**: Returns current system date and time in UTC

### Media Service
- **GetProfiles**: Returns available media profiles with video encoder configurations
- **GetStreamUri**: Returns RTSP stream URI for a given profile
- **GetSnapshotUri**: Returns HTTP snapshot URI for a given profile
- **GetVideoEncoderConfiguration**: Returns video encoder settings

### Authentication & Security
- **WS-UsernameToken**: Full support for username/password authentication
- **Password Digest**: SHA-1 based password digest authentication
- **Nonce Cache**: Replay attack prevention
- **Timestamp Validation**: Prevents replay attacks using timestamp checking

### SOAP Message Handling
- **SOAP 1.2 Parsing**: Full SOAP envelope parsing
- **SOAP Response Generation**: Creates valid SOAP responses
- **SOAP Fault Handling**: Proper error reporting via SOAP faults
- **Namespace Support**: Correct ONVIF namespace handling

## Architecture

```
┌──────────────────────────────────────────────────┐
│              Client Applications                  │
└────────────────────┬─────────────────────────────┘
                     │ SOAP over HTTP
                     ▼
┌──────────────────────────────────────────────────┐
│            ONVIF Server (onvif_server.c)         │
│  - Network listener (socket)                      │
│  - HTTP request handling                          │
│  - Connection management                          │
└────────────────────┬─────────────────────────────┘
                     │
           ┌─────────┴─────────┐
           ▼                   ▼
┌──────────────────┐  ┌──────────────────┐
│  SOAP Handler    │  │  Auth Handler    │
│  - Parse request │  │  - Extract token │
│  - Create response│  │  - Validate      │
│  - Handle faults │  │  - Nonce cache   │
└────────┬─────────┘  └──────────────────┘
         │
    ┌────┴────┬──────────────┐
    ▼         ▼              ▼
┌─────────┐ ┌─────────┐ ┌─────────┐
│ Device  │ │ Media   │ │  PTZ    │
│ Service │ │ Service │ │ Service │
│ Handler │ │ Handler │ │ Handler │
└─────────┘ └─────────┘ └─────────┘
```

## BDD Testing Approach

The server follows Behavior-Driven Development principles:

### 1. Feature Files (Gherkin Syntax)

Feature files describe expected behavior in natural language:

```gherkin
Feature: ONVIF Server Device Management Service
  As an ONVIF server
  I want to handle device management requests
  So that clients can discover device information

  Scenario: Handle GetDeviceInformation request
    Given the ONVIF server is running
    When a client requests device information
    Then the server should return manufacturer information
    And the server should return model information
    And the response should be valid SOAP format
```

### 2. Test Implementation

Tests are implemented in `tests/cucumber_runner.c` with step definitions that verify each scenario:

```c
int test_handle_get_device_information() {
    printf("\n  Scenario: Handle GetDeviceInformation request\n");
    
    setup_test_server();
    
    char response[4096];
    int ret = handle_get_device_information(&test_server, 
                                           response, sizeof(response));
    
    if (ret == ONVIF_SERVER_OK && 
        strstr(response, "TestManufacturer")) {
        printf("    ✓ PASSED\n");
        return 0;
    }
    
    printf("    ✗ FAILED\n");
    return -1;
}
```

### 3. Feature Coverage

The server implementation includes feature files for:

- **server_device_management.feature**: Device service operations
- **server_media_service.feature**: Media service operations
- **server_authentication.feature**: Authentication scenarios
- **server_soap_handling.feature**: SOAP message processing

## Using gSOAP (Optional)

The server is designed to optionally use gSOAP for enhanced SOAP processing:

### Building with gSOAP

```bash
# Install gSOAP
sudo apt-get install gsoap libgsoap-dev

# Build with gSOAP support
mkdir build && cd build
cmake -DUSE_GSOAP=ON ..
make
```

### gSOAP Integration Points

When `USE_GSOAP` is enabled:

1. **WSDL Processing**: Automatic stub generation from ONVIF WSDL files
2. **XML Serialization**: Automatic marshaling/unmarshaling of SOAP messages
3. **Type Safety**: Compile-time type checking for ONVIF operations
4. **Standards Compliance**: Full WS-* standards support

### Without gSOAP

The basic implementation uses:
- Manual SOAP XML parsing and generation
- String-based message handling
- Sufficient for most ONVIF Profile S requirements

## Building and Running

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libssl-dev

# For gSOAP support (optional)
sudo apt-get install gsoap libgsoap-dev
```

### Build Steps

```bash
cd cpp/server
mkdir build && cd build

# Basic build
cmake ..
make

# With gSOAP (optional)
cmake -DUSE_GSOAP=ON ..
make

# Run tests
ctest --verbose
```

### Running the Server

```bash
# Run example server on default port (8080)
./examples/onvif_server_example

# Run on custom port
./examples/onvif_server_example 8888
```

## Using the Server

### Basic Usage

```c
#include "onvif_server.h"

int main() {
    onvif_server_t server;
    
    // Initialize
    onvif_server_init(&server, 8080);
    
    // Configure device info
    onvif_server_set_device_info(&server,
        "My Manufacturer",
        "My Camera Model",
        "1.0.0",
        "SN12345");
    
    // Add authentication
    onvif_server_set_auth_required(&server, true);
    onvif_server_add_user(&server, "admin", "password");
    
    // Add media profile
    onvif_media_profile_t profile;
    memset(&profile, 0, sizeof(profile));
    strcpy(profile.token, "profile_1");
    strcpy(profile.name, "MainStream");
    strcpy(profile.encoding, "H264");
    profile.width = 1920;
    profile.height = 1080;
    profile.frame_rate_limit = 30;
    strcpy(profile.rtsp_uri, "rtsp://192.168.1.100:554/stream1");
    
    onvif_server_add_profile(&server, &profile);
    
    // Start server (blocking)
    onvif_server_start(&server);
    
    // Cleanup
    onvif_server_destroy(&server);
    
    return 0;
}
```

### Configuration Options

#### Device Information
```c
onvif_server_set_device_info(&server,
    manufacturer,      // e.g., "Acme Corp"
    model,            // e.g., "AC-1000"
    firmware_version, // e.g., "1.2.3"
    serial_number);   // e.g., "SN123456789"
```

#### Authentication
```c
// Enable authentication
onvif_server_set_auth_required(&server, true);

// Add users
onvif_server_add_user(&server, "admin", "admin_pass");
onvif_server_add_user(&server, "viewer", "viewer_pass");
```

#### Media Profiles
```c
onvif_media_profile_t profile = {
    .token = "profile_1",
    .name = "HD Stream",
    .encoding = "H264",
    .width = 1920,
    .height = 1080,
    .frame_rate_limit = 30,
    .quality = 5,
    .bitrate_limit = 4000000,
    .rtsp_uri = "rtsp://192.168.1.100:554/stream1",
    .snapshot_uri = "http://192.168.1.100/snapshot.jpg"
};

onvif_server_add_profile(&server, &profile);
```

## Testing Client-Server Integration

### Using the Client Library

```c
// Client code
#include "onvif_client.h"

onvif_client_t client;
onvif_client_init(&client, "http://localhost:8080", "admin", "password");

onvif_device_info_t info;
if (onvif_get_device_information(&client, &info) == ONVIF_OK) {
    printf("Connected to: %s %s\n", info.manufacturer, info.model);
}

onvif_client_destroy(&client);
```

### Testing with External Tools

#### ONVIF Device Manager
1. Download from: https://sourceforge.net/projects/onvifdm/
2. Add device with server address (e.g., `http://localhost:8080`)
3. Enter credentials
4. Test device discovery and media retrieval

#### cURL
```bash
# Get device information
curl -X POST http://localhost:8080/onvif/device_service \
  -H "Content-Type: application/soap+xml" \
  -d '<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope">
  <s:Body>
    <tds:GetDeviceInformation xmlns:tds="http://www.onvif.org/ver10/device/wsdl"/>
  </s:Body>
</s:Envelope>'
```

## API Reference

See `include/onvif_server.h` for complete API documentation.

### Core Functions

- `onvif_server_init()` - Initialize server
- `onvif_server_start()` - Start listening for connections
- `onvif_server_stop()` - Stop server
- `onvif_server_destroy()` - Cleanup resources

### Configuration Functions

- `onvif_server_set_device_info()` - Set device information
- `onvif_server_add_user()` - Add authentication user
- `onvif_server_add_profile()` - Add media profile
- `onvif_server_set_auth_required()` - Enable/disable authentication

## Extending the Server

### Adding New Operations

1. **Define in feature file**:
```gherkin
Scenario: Handle new operation
  Given the ONVIF server is running
  When a client requests new operation
  Then the server should return appropriate response
```

2. **Implement handler**:
```c
int handle_new_operation(onvif_server_t* server,
                        char* response_xml,
                        size_t max_len) {
    // Implementation
    return ONVIF_SERVER_OK;
}
```

3. **Add test**:
```c
int test_handle_new_operation() {
    setup_test_server();
    
    char response[4096];
    int ret = handle_new_operation(&test_server, 
                                   response, sizeof(response));
    
    assert(ret == ONVIF_SERVER_OK);
    return 0;
}
```

## Troubleshooting

### Common Issues

**Server won't start**
- Check if port is already in use: `netstat -an | grep 8080`
- Try a different port
- Check firewall settings

**Authentication failures**
- Verify username/password are correct
- Check timestamp tolerance (default: 5 minutes)
- Ensure system time is synchronized

**SOAP parsing errors**
- Validate XML format
- Check namespace declarations
- Enable verbose logging

### Debug Tips

Enable detailed logging by modifying the source or using a debugger:

```bash
# Run with GDB
gdb ./onvif_server_example
(gdb) break onvif_server_start
(gdb) run
```

## Standards Compliance

This implementation follows:
- **ONVIF Core Specification 2.0+**
- **ONVIF Profile S Specification**
- **SOAP 1.2** (with SOAP 1.1 compatibility)
- **WS-UsernameToken** for authentication
- **HTTP/1.1** for transport

## Future Enhancements

Potential additions:
- [ ] Full gSOAP integration with WSDL stubs
- [ ] PTZ service implementation
- [ ] Event service (notifications)
- [ ] Recording search and playback
- [ ] HTTPS/TLS support
- [ ] WS-Discovery for device discovery
- [ ] Analytics service
- [ ] Multi-threading for concurrent connections
- [ ] Configuration file support (JSON/XML)

## References

- [ONVIF Website](https://www.onvif.org/)
- [ONVIF Profile S Specification](https://www.onvif.org/profiles/profile-s/)
- [gSOAP Toolkit](https://www.genivia.com/dev.html)
- [SOAP 1.2 Specification](https://www.w3.org/TR/soap12/)
- [WS-UsernameToken](http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0.pdf)

## License

This implementation is part of the ipcamera-onvif-client project and follows the same license terms.

## Support

For questions or issues:
- GitHub Issues: https://github.com/kiddy818/ipcamera-onvif-client/issues
- ONVIF Forum: https://www.onvif.org/forum/
