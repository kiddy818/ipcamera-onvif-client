#!/usr/bin/env python3
"""
Example usage of ONVIF Client for IP Cameras
This script demonstrates how to use the ONVIF client to interact with an IP camera
"""

import sys
import time
from onvif_client import ONVIFClient, setup_logging


def main():
    """Main example function"""
    
    # Setup logging
    setup_logging()
    
    # Camera configuration
    # Replace these with your camera's actual credentials
    CAMERA_HOST = '192.168.1.100'  # Camera IP address
    CAMERA_PORT = 80               # ONVIF service port
    USERNAME = 'admin'             # Camera username
    PASSWORD = 'password'          # Camera password
    
    print("=" * 60)
    print("ONVIF Camera Client Example")
    print("=" * 60)
    print()
    
    # Create ONVIF client instance
    print(f"Connecting to camera at {CAMERA_HOST}:{CAMERA_PORT}...")
    client = ONVIFClient(CAMERA_HOST, CAMERA_PORT, USERNAME, PASSWORD)
    
    # Connect to the camera
    if not client.connect():
        print("Failed to connect to camera. Please check your credentials and network connection.")
        sys.exit(1)
    
    print("✓ Successfully connected to camera")
    print()
    
    # Get device information
    print("-" * 60)
    print("Device Information:")
    print("-" * 60)
    device_info = client.get_device_information()
    if device_info:
        for key, value in device_info.items():
            print(f"  {key}: {value}")
    else:
        print("  Failed to retrieve device information")
    print()
    
    # Get device capabilities
    print("-" * 60)
    print("Device Capabilities:")
    print("-" * 60)
    capabilities = client.get_capabilities()
    if capabilities:
        for key, value in capabilities.items():
            status = "✓ Supported" if value else "✗ Not supported"
            print(f"  {key}: {status}")
    else:
        print("  Failed to retrieve capabilities")
    print()
    
    # Get media profiles
    print("-" * 60)
    print("Media Profiles:")
    print("-" * 60)
    profiles = client.get_profiles()
    if profiles:
        for i, profile in enumerate(profiles):
            print(f"  Profile {i + 1}:")
            print(f"    Token: {profile.token}")
            print(f"    Name: {profile.Name}")
            if hasattr(profile, 'VideoEncoderConfiguration') and profile.VideoEncoderConfiguration:
                video_config = profile.VideoEncoderConfiguration
                print(f"    Video Encoding: {video_config.Encoding}")
                print(f"    Resolution: {video_config.Resolution.Width}x{video_config.Resolution.Height}")
                print(f"    Frame Rate: {video_config.RateControl.FrameRateLimit}")
            print()
    else:
        print("  Failed to retrieve media profiles")
    
    # Get stream URI
    print("-" * 60)
    print("Stream URI:")
    print("-" * 60)
    stream_uri = client.get_stream_uri()
    if stream_uri:
        print(f"  RTSP Stream: {stream_uri}")
    else:
        print("  Failed to retrieve stream URI")
    print()
    
    # Get snapshot URI
    print("-" * 60)
    print("Snapshot URI:")
    print("-" * 60)
    snapshot_uri = client.get_snapshot_uri()
    if snapshot_uri:
        print(f"  Snapshot URL: {snapshot_uri}")
    else:
        print("  Failed to retrieve snapshot URI")
    print()
    
    # PTZ demonstration (if supported)
    if capabilities and capabilities.get('PTZ', False):
        print("-" * 60)
        print("PTZ Control Demonstration:")
        print("-" * 60)
        print("  Demonstrating PTZ control...")
        
        # Pan right
        print("  → Moving right...")
        client.move_ptz(pan=0.5, tilt=0.0, zoom=0.0)
        time.sleep(2)
        
        # Stop
        print("  ■ Stopping...")
        client.stop_ptz()
        time.sleep(1)
        
        # Pan left
        print("  ← Moving left...")
        client.move_ptz(pan=-0.5, tilt=0.0, zoom=0.0)
        time.sleep(2)
        
        # Stop
        print("  ■ Stopping...")
        client.stop_ptz()
        time.sleep(1)
        
        # Tilt up
        print("  ↑ Moving up...")
        client.move_ptz(pan=0.0, tilt=0.5, zoom=0.0)
        time.sleep(2)
        
        # Stop
        print("  ■ Stopping...")
        client.stop_ptz()
        
        print("  ✓ PTZ demonstration complete")
    else:
        print("-" * 60)
        print("PTZ Control:")
        print("-" * 60)
        print("  PTZ not supported by this camera")
    
    print()
    print("=" * 60)
    print("Example completed successfully!")
    print("=" * 60)


if __name__ == '__main__':
    main()
