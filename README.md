# ONVIF Profile S Client Implementation

A C/C++ implementation of ONVIF Profile S (Basic Video Device Specification) using requirements-driven development with BDD testing.

## Overview

This project implements the ONVIF Profile S specification for communicating with IP cameras and video devices. It provides a clean C API for:

- **Device Management Service**: Get device information, capabilities, and available services
- **Media Service**: Retrieve media profiles, stream URIs, and snapshot URIs  
- **PTZ Service**: Control Pan-Tilt-Zoom functionality (optional in Profile S)
- **Authentication**: WS-UsernameToken security support
- **Network Communication**: HTTP/SOAP messaging with CURL

## Features

✅ **ONVIF Profile S Compliant** - Implements core Profile S requirements  
✅ **Requirements-Driven Development** - Features defined as Cucumber BDD scenarios  
✅ **Comprehensive Testing** - Unit tests and BDD acceptance tests  
✅ **Clean C API** - Simple, well-documented interface  
✅ **Cross-Platform** - Works on Linux, macOS, and Windows

## Requirements

### Build Dependencies
- CMake 3.10 or higher
- C compiler (GCC, Clang, or MSVC)
- libcurl development package

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libcurl4-openssl-dev
```

### macOS
```bash
brew install cmake curl
```

### Fedora/RHEL
```bash
sudo dnf install cmake gcc libcurl-devel
```

## Building

```bash
# Clone the repository
git clone https://github.com/kiddy818/ipcamera-onvif-client.git
cd ipcamera-onvif-client

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run tests
ctest --verbose
```

## Usage

### Basic Example

```c
#include "onvif_client.h"
#include <stdio.h>

int main() {
    // Initialize client
    onvif_client_t client;
    onvif_client_init(&client, "http://192.168.1.100", "admin", "password");
    
    // Get device information
    onvif_device_info_t info;
    if (onvif_get_device_information(&client, &info) == ONVIF_OK) {
        printf("Manufacturer: %s\n", info.manufacturer);
        printf("Model: %s\n", info.model);
        printf("Firmware: %s\n", info.firmware_version);
    }
    
    // Get media profiles
    onvif_media_profile_t profiles[ONVIF_MAX_PROFILES];
    int count = 0;
    onvif_get_profiles(&client, profiles, &count, ONVIF_MAX_PROFILES);
    
    // Get stream URI
    if (count > 0) {
        onvif_stream_uri_t stream_uri;
        onvif_get_stream_uri(&client, profiles[0].token, &stream_uri);
        printf("Stream URI: %s\n", stream_uri.uri);
    }
    
    // Cleanup
    onvif_client_destroy(&client);
    return 0;
}
```

### Running the Example

```bash
./build/examples/onvif_example http://192.168.1.100 admin password123
```

## BDD Testing

The project uses Behavior-Driven Development (BDD) with Cucumber-style feature files to ensure requirements are properly validated.

### Feature Files

Feature files are located in `features/` and describe the expected behavior:

- `device_management.feature` - Device discovery and information retrieval
- `media_service.feature` - Media profiles and streaming
- `ptz_service.feature` - Pan-Tilt-Zoom control
- `authentication.feature` - Security and authentication

### Running BDD Tests

```bash
cd build
./tests/test_cucumber_runner
```

### Sample Feature

```gherkin
Feature: ONVIF Device Management Service
  As an ONVIF client
  I want to communicate with an ONVIF-compliant IP camera
  So that I can retrieve device information and capabilities

  Scenario: Get device information
    Given an initialized ONVIF client
    When I request device information
    Then I should receive manufacturer information
    And I should receive model information
    And I should receive firmware version
```

## API Reference

### Client Initialization

```c
int onvif_client_init(onvif_client_t* client, const char* device_url,
                      const char* username, const char* password);
void onvif_client_destroy(onvif_client_t* client);
```

### Device Management Service

```c
int onvif_get_device_information(onvif_client_t* client, onvif_device_info_t* info);
int onvif_get_capabilities(onvif_client_t* client, onvif_capabilities_t* caps);
int onvif_get_services(onvif_client_t* client, char* services, size_t max_len);
```

### Media Service

```c
int onvif_get_profiles(onvif_client_t* client, onvif_media_profile_t* profiles,
                       int* count, int max_profiles);
int onvif_get_stream_uri(onvif_client_t* client, const char* profile_token,
                         onvif_stream_uri_t* stream_uri);
int onvif_get_snapshot_uri(onvif_client_t* client, const char* profile_token,
                           char* uri, size_t max_len);
```

### Error Codes

```c
#define ONVIF_OK                0   // Success
#define ONVIF_ERR_INVALID_PARAM -1  // Invalid parameter
#define ONVIF_ERR_NETWORK       -2  // Network error
#define ONVIF_ERR_AUTH          -3  // Authentication failure
#define ONVIF_ERR_PARSE         -4  // Parse error
#define ONVIF_ERR_NOT_SUPPORTED -5  // Feature not supported
```

## Project Structure

```
.
├── CMakeLists.txt           # Main build configuration
├── README.md                # This file
├── include/                 # Public headers
│   ├── onvif_client.h      # Main client API
│   ├── soap_handler.h      # SOAP message handling
│   └── http_client.h       # HTTP communication
├── src/                     # Implementation
│   ├── onvif_client.c      # Client initialization
│   ├── device_service.c    # Device Management Service
│   ├── media_service.c     # Media Service
│   ├── ptz_service.c       # PTZ Service
│   ├── soap_handler.c      # SOAP implementation
│   ├── http_client.c       # HTTP/CURL wrapper
│   └── auth.c              # Authentication
├── tests/                   # Test suite
│   ├── test_onvif_client.c
│   ├── test_device_service.c
│   ├── test_media_service.c
│   └── cucumber_runner.c   # BDD test runner
├── features/                # BDD feature files
│   ├── device_management.feature
│   ├── media_service.feature
│   ├── ptz_service.feature
│   └── authentication.feature
└── examples/                # Example applications
    └── onvif_example.c
```

## ONVIF Profile S Coverage

This implementation covers the following ONVIF Profile S requirements:

### Device Management Service
- ✅ GetDeviceInformation
- ✅ GetCapabilities  
- ✅ GetServices
- ⚠️  GetSystemDateAndTime (not yet implemented)
- ⚠️  SetSystemDateAndTime (not yet implemented)

### Media Service
- ✅ GetProfiles
- ✅ GetStreamUri
- ✅ GetSnapshotUri
- ⚠️  GetVideoEncoderConfiguration (not yet implemented)
- ⚠️  GetVideoEncoderConfigurationOptions (not yet implemented)

### Security
- ✅ WS-UsernameToken support via HTTP Digest/Basic auth
- ⚠️  Full WS-Security implementation (in progress)

## Testing Strategy

The project follows requirements-driven development:

1. **Feature Definition**: Requirements are defined as Cucumber feature files
2. **Test Implementation**: Step definitions implement the test scenarios
3. **Code Implementation**: Features are implemented to satisfy tests
4. **Validation**: Tests verify that requirements are met

### Test Types

- **Unit Tests**: Test individual components in isolation
- **Integration Tests**: Test component interactions
- **BDD Tests**: Validate requirements from user perspective
- **Conformance Tests**: Validate ONVIF standard compliance

## Contributing

Contributions are welcome! Please follow these guidelines:

1. Write tests first (TDD/BDD approach)
2. Ensure all tests pass before submitting PR
3. Follow existing code style
4. Update documentation for new features

## License

This project is open source. Please check the LICENSE file for details.

## References

- [ONVIF Profile S Specification](https://www.onvif.org/profiles/profile-s/)
- [ONVIF Application Programmer's Guide](https://www.onvif.org/specs/DocMap-2.4.2.html)
- [ONVIF Test Tool](https://www.onvif.org/conformance/testing/)

## Support

For issues and questions:
- GitHub Issues: https://github.com/kiddy818/ipcamera-onvif-client/issues
- ONVIF Forum: https://www.onvif.org/forum/