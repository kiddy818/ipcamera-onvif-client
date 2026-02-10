# ONVIF Profile S Implementation Guide

## What is ONVIF Profile S?

ONVIF Profile S is a specification that defines the requirements for IP-based video systems, particularly for streaming video from network cameras. It's the most basic and widely-supported ONVIF profile.

## Profile S Core Requirements

### 1. Device Management Service

The Device Management Service provides access to device information and configuration:

- **GetDeviceInformation**: Retrieve manufacturer, model, firmware version, serial number
- **GetCapabilities**: Discover which services the device supports
- **GetServices**: Get detailed information about available services and their endpoints
- **GetSystemDateAndTime**: Get device's current date and time
- **SetSystemDateAndTime**: Configure device's date and time

### 2. Media Service

The Media Service handles video configuration and streaming:

- **GetProfiles**: Retrieve available media profiles (video encoder + source configurations)
- **GetStreamUri**: Get RTSP URI for video streaming
- **GetSnapshotUri**: Get HTTP URI for JPEG snapshots
- **GetVideoEncoderConfiguration**: Get video encoding settings
- **GetVideoEncoderConfigurationOptions**: Get supported encoding options

### 3. Security

Profile S requires WS-UsernameToken authentication:
- Username/password credentials
- Password digest (SHA-1 hash)
- Nonce and timestamp for replay protection

## Implementation Details

### Architecture

```
┌─────────────────────┐
│  Client Application │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│   ONVIF Client API  │
│  (onvif_client.h)   │
└──────────┬──────────┘
           │
     ┌─────┴─────┬──────────┬──────────┐
     ▼           ▼          ▼          ▼
┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐
│ Device │  │ Media  │  │  PTZ   │  │  Auth  │
│Service │  │Service │  │Service │  │        │
└────┬───┘  └────┬───┘  └────┬───┘  └────┬───┘
     └───────────┴───────────┴───────────┘
                    ▼
     ┌──────────────────────────────────┐
     │      SOAP Handler + HTTP Client  │
     └──────────────────────────────────┘
                    ▼
     ┌──────────────────────────────────┐
     │         Network (libcurl)        │
     └──────────────────────────────────┘
```

### SOAP Message Flow

1. **Client** creates a request using the API
2. **Service layer** constructs SOAP body
3. **SOAP handler** wraps body in SOAP envelope
4. **HTTP client** sends POST request with authentication
5. **Network layer** (libcurl) handles HTTP/TCP
6. **HTTP client** receives response
7. **SOAP handler** parses response XML
8. **Service layer** extracts data
9. **Client** receives structured data

### Example SOAP Request

```xml
<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope">
  <s:Header>
    <Security xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd">
      <UsernameToken>
        <Username>admin</Username>
        <Password Type="PasswordDigest">...</Password>
        <Nonce>...</Nonce>
        <Created>2024-01-01T12:00:00Z</Created>
      </UsernameToken>
    </Security>
  </s:Header>
  <s:Body>
    <GetDeviceInformation xmlns="http://www.onvif.org/ver10/device/wsdl"/>
  </s:Body>
</s:Envelope>
```

## Testing Strategy

### Requirements-Driven Development

This implementation follows BDD (Behavior-Driven Development) principles:

1. **Define Requirements**: Written as Cucumber feature files in Gherkin syntax
2. **Implement Tests**: Step definitions that verify requirements
3. **Write Code**: Implementation to satisfy tests
4. **Validate**: Run tests to ensure requirements are met

### Test Hierarchy

```
Feature Files (features/)
    ├── Describe user-facing requirements
    └── Written in natural language (Gherkin)
              ↓
BDD Tests (tests/cucumber_runner.c)
    ├── Implement scenarios from feature files
    └── Integration-level testing
              ↓
Unit Tests (tests/test_*.c)
    ├── Test individual functions
    └── Verify error handling and edge cases
```

### Example Feature

```gherkin
Feature: ONVIF Device Management Service
  As an ONVIF client
  I want to retrieve device information
  So that I can identify the camera

  Scenario: Get device information
    Given an initialized ONVIF client
    When I request device information
    Then I should receive manufacturer information
    And I should receive model information
    And I should receive firmware version
```

## Common Use Cases

### 1. Discover and Connect to Camera

```c
onvif_client_t client;
onvif_client_init(&client, "http://192.168.1.100", "admin", "password");

// Get device info
onvif_device_info_t info;
onvif_get_device_information(&client, &info);
printf("Camera: %s %s\n", info.manufacturer, info.model);
```

### 2. Get Video Stream URI

```c
// Get profiles
onvif_media_profile_t profiles[ONVIF_MAX_PROFILES];
int count = 0;
onvif_get_profiles(&client, profiles, &count, ONVIF_MAX_PROFILES);

// Get stream URI
onvif_stream_uri_t stream_uri;
onvif_get_stream_uri(&client, profiles[0].token, &stream_uri);

// Use URI with VLC, FFmpeg, or your player
printf("RTSP Stream: %s\n", stream_uri.uri);
```

### 3. Take Snapshot

```c
char snapshot_uri[ONVIF_MAX_URL_LEN];
onvif_get_snapshot_uri(&client, profiles[0].token, 
                       snapshot_uri, sizeof(snapshot_uri));

// Download JPEG using HTTP GET
// curl_easy_perform() or similar
```

## Extending the Implementation

### Adding New Features

1. **Define requirement** in feature file
2. **Add test** to cucumber_runner.c or create new test file
3. **Implement function** in appropriate service file
4. **Add to API** in onvif_client.h if public
5. **Run tests** to verify

### Example: Add GetSystemDateAndTime

1. Add to `features/device_management.feature`:
```gherkin
Scenario: Get system date and time
  Given an initialized ONVIF client
  When I request system date and time
  Then I should receive current date and time
```

2. Add function to `src/device_service.c`:
```c
int onvif_get_system_date_time(onvif_client_t* client, 
                               onvif_date_time_t* dt);
```

3. Add test and verify

## Troubleshooting

### Common Issues

**Problem**: Network timeout
**Solution**: Increase client->timeout_ms or check network connectivity

**Problem**: Authentication failed
**Solution**: Verify username/password, check if device requires WS-Security

**Problem**: Parse error
**Solution**: Check SOAP response format, may need full XML parser

**Problem**: Empty response
**Solution**: Verify device URL, check if service is available

### Debug Tips

1. Enable verbose curl output:
```c
curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
```

2. Print SOAP messages:
```c
printf("Request: %s\n", request.xml_data);
printf("Response: %s\n", response.body);
```

3. Test with real ONVIF device or simulator
4. Use ONVIF Device Test Tool for validation

## Standards Compliance

This implementation follows:
- ONVIF Core Specification v2.0+
- ONVIF Profile S Specification
- WS-Discovery 1.0
- SOAP 1.2
- HTTP/1.1

## Future Enhancements

Potential additions:
- [ ] Full WS-Security implementation
- [ ] WS-Discovery for automatic device detection  
- [ ] Event service for notifications
- [ ] Recording search and playback
- [ ] Audio support
- [ ] Metadata streaming
- [ ] Full XML parser (libxml2 integration)
- [ ] HTTPS/TLS support
- [ ] ONVIF Profile T (Video Streaming)
- [ ] ONVIF Profile C (Access Control)

## References

- [ONVIF Website](https://www.onvif.org/)
- [ONVIF Profile S Specification](https://www.onvif.org/profiles/profile-s/)
- [ONVIF Application Programmer's Guide](https://www.onvif.org/specs/DocMap-2.4.2.html)
- [ONVIF Test Tool](https://www.onvif.org/conformance/testing/)
- [libcurl Documentation](https://curl.se/libcurl/)
