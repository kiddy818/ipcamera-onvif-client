# CPP VERSION: ONVIF Profile S Client Implementation

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
