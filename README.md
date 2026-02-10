# IP Camera ONVIF Client

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
- Create an issue on GitHub
- Check the troubleshooting section above
- Refer to your camera's ONVIF documentation