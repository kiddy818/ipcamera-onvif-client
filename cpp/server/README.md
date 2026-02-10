# ONVIF Profile S Server

A complete ONVIF Profile S server implementation using BDD (Behavior-Driven Development) approach with optional gSOAP support.

## Features

✅ **Device Management Service** - GetDeviceInformation, GetCapabilities, GetServices, GetSystemDateAndTime  
✅ **Media Service** - GetProfiles, GetStreamUri, GetSnapshotUri, GetVideoEncoderConfiguration  
✅ **Authentication** - WS-UsernameToken with password digest and replay protection  
✅ **SOAP 1.2** - Full SOAP envelope parsing and response generation  
✅ **BDD Testing** - Comprehensive behavior-driven tests with Gherkin-style scenarios  
✅ **gSOAP Support** - Optional integration with gSOAP toolkit

## Quick Start

### Build

```bash
cd cpp/server
mkdir build && cd build
cmake ..
make
```

### Run Tests

```bash
ctest --verbose
```

### Run Example Server

```bash
./examples/onvif_server_example 8080
```

The server will start on port 8080 with:
- **Users**: admin/admin123, operator/oper123
- **Device Service**: http://localhost:8080/onvif/device_service
- **Media Service**: http://localhost:8080/onvif/media_service

## Using gSOAP (Optional)

For enhanced SOAP processing with full WS-* standards support:

```bash
# Install gSOAP
sudo apt-get install gsoap libgsoap-dev

# Build with gSOAP
cmake -DUSE_GSOAP=ON ..
make
```

## Testing with Client

You can test the server using the included client library:

```c
#include "onvif_client.h"

onvif_client_t client;
onvif_client_init(&client, "http://localhost:8080", "admin", "admin123");

onvif_device_info_t info;
onvif_get_device_information(&client, &info);
printf("Device: %s %s\n", info.manufacturer, info.model);
```

## BDD Test Results

All 11 BDD tests pass:

```
Feature: Server Device Management Service
  ✓ Server initialization
  ✓ Handle GetDeviceInformation
  ✓ Handle GetCapabilities
  ✓ Handle GetServices
  ✓ Handle GetSystemDateAndTime

Feature: Server Media Service
  ✓ Handle GetProfiles
  ✓ Handle GetStreamUri

Feature: SOAP Message Handling
  ✓ Parse SOAP request
  ✓ Create SOAP response
  ✓ Create SOAP fault

Feature: Authentication
  ✓ Add user
```

## Documentation

See [SERVER_IMPLEMENTATION_GUIDE.md](SERVER_IMPLEMENTATION_GUIDE.md) for complete documentation including:
- Architecture overview
- BDD testing approach
- API reference
- Usage examples
- gSOAP integration
- Extending the server

## Directory Structure

```
cpp/server/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── SERVER_IMPLEMENTATION_GUIDE.md  # Detailed documentation
├── include/                    # Public headers
│   ├── onvif_server.h         # Main server API
│   ├── device_service_handler.h
│   ├── media_service_handler.h
│   ├── soap_server_handler.h
│   └── auth_handler.h
├── src/                        # Implementation
│   ├── onvif_server.c
│   ├── device_service_handler.c
│   ├── media_service_handler.c
│   ├── soap_server_handler.c
│   └── auth_handler.c
├── features/                   # BDD feature files
│   ├── server_device_management.feature
│   ├── server_media_service.feature
│   ├── server_authentication.feature
│   └── server_soap_handling.feature
├── tests/                      # BDD tests
│   ├── CMakeLists.txt
│   └── cucumber_runner.c
└── examples/                   # Example applications
    ├── CMakeLists.txt
    └── server_example.c
```

## Standards Compliance

- ONVIF Core Specification 2.0+
- ONVIF Profile S Specification
- SOAP 1.2 (with SOAP 1.1 compatibility)
- WS-UsernameToken authentication
- HTTP/1.1 transport

## License

Part of the ipcamera-onvif-client project.
