# ONVIF Profile S Client & Server Implementation

A complete C/C++ implementation of ONVIF Profile S (Basic Video Device Specification) including both **client** and **server** components, developed using requirements-driven development with BDD testing.

## Overview

This project provides a complete ONVIF Profile S implementation:

### Client Library (`cpp/client/`)
- **Device Management Service**: Get device information, capabilities, and available services
- **Media Service**: Retrieve media profiles, stream URIs, and snapshot URIs  
- **PTZ Service**: Control Pan-Tilt-Zoom functionality (optional in Profile S)
- **Authentication**: WS-UsernameToken security support
- **Network Communication**: HTTP/SOAP messaging with CURL

### Server Library (`cpp/server/`) **NEW!**
- **Device Management Service**: Handle GetDeviceInformation, GetCapabilities, GetServices, GetSystemDateAndTime
- **Media Service**: Handle GetProfiles, GetStreamUri, GetSnapshotUri, GetVideoEncoderConfiguration
- **Authentication**: WS-UsernameToken with password digest and replay protection
- **SOAP 1.2 Support**: Full SOAP envelope parsing and response generation
- **gSOAP Integration**: Optional gSOAP support for enhanced SOAP processing

## Features

✅ **Complete ONVIF Solution** - Both client and server implementations  
✅ **ONVIF Profile S Compliant** - Implements core Profile S requirements  
✅ **Requirements-Driven Development** - Features defined as Cucumber BDD scenarios  
✅ **Comprehensive Testing** - Unit tests and BDD acceptance tests  
✅ **Clean C API** - Simple, well-documented interface  
✅ **Cross-Platform** - Works on Linux, macOS, and Windows  
✅ **gSOAP Support** - Optional integration with gSOAP toolkit

## Requirements

### Build Dependencies
- CMake 3.10 or higher
- C compiler (GCC, Clang, or MSVC)
- libcurl development package (for client)
- OpenSSL development package (for server authentication)
- gSOAP toolkit (optional, for enhanced server SOAP processing)

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libcurl4-openssl-dev libssl-dev

# Optional: for gSOAP support
sudo apt-get install gsoap libgsoap-dev
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

### Running the Client Example

```bash
./build/client/examples/onvif_example http://192.168.1.100 admin password123
```

## Server Usage

### Basic Server Example

```c
#include "onvif_server.h"
#include <stdio.h>

int main() {
    onvif_server_t server;
    
    // Initialize server
    onvif_server_init(&server, 8080);
    
    // Configure device info
    onvif_server_set_device_info(&server,
        "Example Manufacturer",
        "IP Camera Model",
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
    printf("Starting ONVIF server on port 8080...\n");
    onvif_server_start(&server);
    
    // Cleanup
    onvif_server_destroy(&server);
    return 0;
}
```

### Running the Server Example

```bash
cd cpp/server/build
./examples/onvif_server_example 8080
```

The server will start with:
- Device Service: `http://localhost:8080/onvif/device_service`
- Media Service: `http://localhost:8080/onvif/media_service`
- Default credentials: admin/admin123, operator/oper123

### Testing Client-Server Integration

```bash
# Terminal 1: Start server
cd cpp/server/build
./examples/onvif_server_example 8080

# Terminal 2: Test with client
cd cpp/client/build
./examples/onvif_example http://localhost:8080 admin admin123
```

## BDD Testing

Both client and server implementations use Behavior-Driven Development (BDD) with Cucumber-style feature files to ensure requirements are properly validated.

### Client Feature Files

Feature files for the client are located in `cpp/client/features/`:

- `device_management.feature` - Device discovery and information retrieval
- `media_service.feature` - Media profiles and streaming
- `ptz_service.feature` - Pan-Tilt-Zoom control
- `authentication.feature` - Security and authentication

### Server Feature Files

Feature files for the server are located in `cpp/server/features/`:

- `server_device_management.feature` - Server-side device service handling
- `server_media_service.feature` - Server-side media service handling
- `server_authentication.feature` - Server authentication and security
- `server_soap_handling.feature` - SOAP message processing

### Running BDD Tests

```bash
# Client tests
cd cpp/client/build
ctest --verbose

# Server tests
cd cpp/server/build
ctest --verbose
```

All 11 server BDD tests pass:
```
✓ Server initialization
✓ Handle GetDeviceInformation
✓ Handle GetCapabilities
✓ Handle GetServices
✓ Handle GetSystemDateAndTime
✓ Handle GetProfiles
✓ Handle GetStreamUri
✓ Parse SOAP request
✓ Create SOAP response
✓ Create SOAP fault
✓ Add user
```

### Sample Feature

**Client Feature:**
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

**Server Feature:**
```gherkin
Feature: ONVIF Server Device Management Service
  As an ONVIF server
  I want to handle device management requests from clients
  So that clients can discover device information and capabilities

  Scenario: Handle GetDeviceInformation request
    Given the ONVIF server is running
    When a client requests device information
    Then the server should return manufacturer information
    And the server should return model information
    And the response should be valid SOAP format
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
├── README.md                    # This file
├── cpp/
│   ├── client/                  # ONVIF Client Implementation
│   │   ├── CMakeLists.txt      # Client build configuration
│   │   ├── include/            # Public headers
│   │   │   ├── onvif_client.h # Main client API
│   │   │   ├── soap_handler.h # SOAP message handling
│   │   │   └── http_client.h  # HTTP communication
│   │   ├── src/                # Implementation
│   │   │   ├── onvif_client.c # Client initialization
│   │   │   ├── device_service.c # Device Management Service
│   │   │   ├── media_service.c  # Media Service
│   │   │   ├── ptz_service.c    # PTZ Service
│   │   │   ├── soap_handler.c   # SOAP implementation
│   │   │   ├── http_client.c    # HTTP/CURL wrapper
│   │   │   └── auth.c           # Authentication
│   │   ├── tests/              # Test suite
│   │   │   ├── test_onvif_client.c
│   │   │   ├── test_device_service.c
│   │   │   ├── test_media_service.c
│   │   │   └── cucumber_runner.c # BDD test runner
│   │   ├── features/           # BDD feature files
│   │   │   ├── device_management.feature
│   │   │   ├── media_service.feature
│   │   │   ├── ptz_service.feature
│   │   │   └── authentication.feature
│   │   └── examples/           # Example applications
│   │       └── onvif_example.c
│   │
│   └── server/                 # ONVIF Server Implementation (NEW!)
│       ├── CMakeLists.txt      # Server build configuration
│       ├── README.md           # Server documentation
│       ├── SERVER_IMPLEMENTATION_GUIDE.md # Detailed guide
│       ├── include/            # Public headers
│       │   ├── onvif_server.h # Main server API
│       │   ├── device_service_handler.h
│       │   ├── media_service_handler.h
│       │   ├── soap_server_handler.h
│       │   └── auth_handler.h
│       ├── src/                # Implementation
│       │   ├── onvif_server.c
│       │   ├── device_service_handler.c
│       │   ├── media_service_handler.c
│       │   ├── soap_server_handler.c
│       │   └── auth_handler.c
│       ├── tests/              # BDD tests
│       │   └── cucumber_runner.c
│       ├── features/           # BDD feature files
│       │   ├── server_device_management.feature
│       │   ├── server_media_service.feature
│       │   ├── server_authentication.feature
│       │   └── server_soap_handling.feature
│       └── examples/           # Example applications
│           └── server_example.c
│
└── python/                     # Python ONVIF client
    └── onvif_client.py
```

## ONVIF Profile S Coverage

### Client Implementation

The client library implements the following ONVIF Profile S operations:

#### Device Management Service
- ✅ GetDeviceInformation
- ✅ GetCapabilities  
- ✅ GetServices
- ⚠️  GetSystemDateAndTime (not yet implemented)
- ⚠️  SetSystemDateAndTime (not yet implemented)

#### Media Service
- ✅ GetProfiles
- ✅ GetStreamUri
- ✅ GetSnapshotUri
- ⚠️  GetVideoEncoderConfiguration (not yet implemented)
- ⚠️  GetVideoEncoderConfigurationOptions (not yet implemented)

#### Security
- ✅ WS-UsernameToken support via HTTP Digest/Basic auth
- ⚠️  Full WS-Security implementation (in progress)

### Server Implementation

The server library implements the following ONVIF Profile S handlers:

#### Device Management Service
- ✅ GetDeviceInformation
- ✅ GetCapabilities  
- ✅ GetServices
- ✅ GetSystemDateAndTime
- ⚠️  SetSystemDateAndTime (not yet implemented)

#### Media Service
- ✅ GetProfiles
- ✅ GetStreamUri
- ✅ GetSnapshotUri
- ✅ GetVideoEncoderConfiguration
- ⚠️  SetVideoEncoderConfiguration (not yet implemented)

#### Security
- ✅ WS-UsernameToken authentication
- ✅ Password digest (SHA-1 based)
- ✅ Nonce-based replay attack prevention
- ✅ Timestamp validation
- ⚠️  Full WS-Security (in progress)

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
=======


# PYTHON VERSION: IP Camera ONVIF Client

A basic Python implementation of an ONVIF (Open Network Video Interface Forum) client for IP cameras. This client provides easy-to-use interfaces for connecting to and controlling ONVIF-compliant IP cameras.

## Features

- ✅ Device connection and authentication
- ✅ Device information retrieval
- ✅ Device capabilities discovery
- ✅ Media profiles management
- ✅ RTSP stream URI retrieval
- ✅ Snapshot URI retrieval
- ✅ PTZ (Pan-Tilt-Zoom) control
- ✅ Comprehensive error handling and logging

## Requirements

- Python 3.6+
- onvif-zeep library
- zeep library

## Installation

1. Clone this repository:
```bash
git clone https://github.com/kiddy818/ipcamera-onvif-client.git
cd ipcamera-onvif-client
```

2. Install dependencies:
```bash
pip install -r requirements.txt
```

## Quick Start

### Basic Usage

```python
from onvif_client import ONVIFClient, setup_logging

# Setup logging
setup_logging()

# Create client instance
client = ONVIFClient(
    host='192.168.1.100',
    port=80,
    username='admin',
    password='password'
)

# Connect to camera
if client.connect():
    # Get device information
    device_info = client.get_device_information()
    print(f"Camera: {device_info['Manufacturer']} {device_info['Model']}")
    
    # Get stream URI
    stream_uri = client.get_stream_uri()
    print(f"Stream: {stream_uri}")
```

### Running the Example

The example script can read configuration from either a config file or environment variables.

**Option 1: Using configuration file**

1. Copy the example configuration:
```bash
cp config.ini.example config.ini
```

2. Edit `config.ini` with your camera's credentials and IP address

3. Run the example script:
```bash
python example.py
```

**Option 2: Using environment variables**

```bash
export ONVIF_HOST=192.168.1.100
export ONVIF_PORT=80
export ONVIF_USERNAME=admin
export ONVIF_PASSWORD=your_password
export LOG_LEVEL=INFO

python example.py
```

The example script demonstrates:
- Connecting to the camera
- Retrieving device information
- Getting device capabilities
- Listing media profiles
- Retrieving stream and snapshot URIs
- PTZ control (if supported)

## API Reference

### ONVIFClient Class

#### Initialization

```python
client = ONVIFClient(host, port, username, password)
```

**Parameters:**
- `host` (str): IP address or hostname of the camera
- `port` (int): ONVIF service port (usually 80, 8080, or 8000)
- `username` (str): Camera username
- `password` (str): Camera password

#### Methods

##### connect()
Connect to the ONVIF camera and initialize services.

**Returns:** `bool` - True if connection successful, False otherwise

```python
if client.connect():
    print("Connected successfully")
```

##### get_device_information()
Get device information from the camera.

**Returns:** `dict` - Device information including manufacturer, model, firmware version, etc.

```python
info = client.get_device_information()
print(f"Model: {info['Model']}")
print(f"Firmware: {info['FirmwareVersion']}")
```

##### get_capabilities()
Get device capabilities.

**Returns:** `dict` - Dictionary of capabilities and their availability

```python
caps = client.get_capabilities()
if caps['PTZ']:
    print("PTZ is supported")
```

##### get_profiles()
Get media profiles from the camera.

**Returns:** `list` - List of media profiles

```python
profiles = client.get_profiles()
for profile in profiles:
    print(f"Profile: {profile.Name} (Token: {profile.token})")
```

##### get_stream_uri(profile_token=None)
Get the RTSP stream URI for a profile.

**Parameters:**
- `profile_token` (str, optional): Token of the profile. If None, uses the first available profile.

**Returns:** `str` - RTSP stream URI

```python
stream_uri = client.get_stream_uri()
print(f"Stream URI: {stream_uri}")
```

##### get_snapshot_uri(profile_token=None)
Get the snapshot URI for a profile.

**Parameters:**
- `profile_token` (str, optional): Token of the profile. If None, uses the first available profile.

**Returns:** `str` - Snapshot URI

```python
snapshot_uri = client.get_snapshot_uri()
print(f"Snapshot URI: {snapshot_uri}")
```

##### move_ptz(pan, tilt, zoom, profile_token=None)
Move PTZ (Pan-Tilt-Zoom) camera.

**Parameters:**
- `pan` (float): Pan value (-1.0 to 1.0, negative is left, positive is right)
- `tilt` (float): Tilt value (-1.0 to 1.0, negative is down, positive is up)
- `zoom` (float): Zoom value (-1.0 to 1.0, negative is zoom out, positive is zoom in)
- `profile_token` (str, optional): Profile token to use

**Returns:** `bool` - True if successful, False otherwise

```python
# Pan right
client.move_ptz(pan=0.5, tilt=0.0, zoom=0.0)

# Tilt up
client.move_ptz(pan=0.0, tilt=0.5, zoom=0.0)

# Zoom in
client.move_ptz(pan=0.0, tilt=0.0, zoom=0.5)
```

##### stop_ptz(profile_token=None)
Stop PTZ movement.

**Parameters:**
- `profile_token` (str, optional): Profile token to use

**Returns:** `bool` - True if successful, False otherwise

```python
client.stop_ptz()
```

## Configuration

You can use a configuration file for camera settings. See `config.ini.example` for the template:

```ini
[camera]
host = 192.168.1.100
port = 80
username = admin
password = password

[settings]
log_level = INFO
```

## Supported ONVIF Features

This client implements the following ONVIF services:

- **Device Management Service**: Device information, capabilities
- **Media Service**: Media profiles, stream URIs, snapshot URIs
- **PTZ Service**: Pan, tilt, zoom control (if supported by camera)

## Troubleshooting

### Connection Issues

1. **Verify camera IP and port**: Make sure the camera is accessible on the network
   ```bash
   ping 192.168.1.100
   ```

2. **Check ONVIF port**: Common ONVIF ports are 80, 8080, 8000, or 8899

3. **Verify credentials**: Ensure username and password are correct

4. **Firewall**: Make sure firewall is not blocking ONVIF communication

### Common Error Messages

- **"Failed to connect to camera"**: Check IP address, port, and network connectivity
- **"PTZ service not available"**: Camera does not support PTZ or PTZ is disabled
- **"SOAP Fault"**: Authentication failure or incorrect ONVIF command

### Enable Debug Logging

For detailed troubleshooting information:

```python
import logging
from onvif_client import setup_logging

setup_logging(level=logging.DEBUG)
```

## Examples

### Get Multiple Stream URIs

```python
client = ONVIFClient('192.168.1.100', 80, 'admin', 'password')
client.connect()

profiles = client.get_profiles()
for profile in profiles:
    stream_uri = client.get_stream_uri(profile.token)
    print(f"Profile {profile.Name}: {stream_uri}")
```

### PTZ Control with Timing

```python
import time

client = ONVIFClient('192.168.1.100', 80, 'admin', 'password')
client.connect()

# Pan right for 3 seconds
client.move_ptz(pan=0.5, tilt=0.0, zoom=0.0)
time.sleep(3)
client.stop_ptz()

# Tilt up for 2 seconds
client.move_ptz(pan=0.0, tilt=0.5, zoom=0.0)
time.sleep(2)
client.stop_ptz()
```

### Using with VLC or FFmpeg

Once you have the stream URI, you can use it with media players:

**VLC:**
```bash
vlc "rtsp://192.168.1.100:554/stream1"
```

**FFmpeg:**
```bash
ffmpeg -i "rtsp://192.168.1.100:554/stream1" -c copy output.mp4
```

## ONVIF Specification

This client implements the ONVIF specification for IP-based security products. For more information about ONVIF:
- Official website: https://www.onvif.org/
- Specifications: https://www.onvif.org/profiles/

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.


## Support

For issues and questions:

- GitHub Issues: https://github.com/kiddy818/ipcamera-onvif-client/issues
- ONVIF Forum: https://www.onvif.org/forum/
=======
- Create an issue on GitHub
- Check the troubleshooting section above
- Refer to your camera's ONVIF documentation
