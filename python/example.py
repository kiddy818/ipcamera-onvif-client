#!/usr/bin/env python3
"""
Example usage of ONVIF Client for IP Cameras
This script demonstrates how to use the ONVIF client to interact with an IP camera
"""

import sys
import time
import os
from configparser import ConfigParser
from onvif_client import ONVIFClient, setup_logging


def load_config():
    """
    Load camera configuration from config.ini file or environment variables
    
    Returns:
        dict: Configuration dictionary with camera settings
    """
    config = {}
    
    # Try to load from config file first
    config_file = 'config.ini'
    if os.path.exists(config_file):
        parser = ConfigParser()
        parser.read(config_file)
        
        if parser.has_section('camera'):
            config['host'] = parser.get('camera', 'host')
            config['port'] = parser.getint('camera', 'port')
            config['username'] = parser.get('camera', 'username')
            config['password'] = parser.get('camera', 'password')
            
            if parser.has_section('settings'):
                log_level = parser.get('settings', 'log_level', fallback='INFO')
                config['log_level'] = log_level
            return config
    
    # Fall back to environment variables
    config['host'] = os.getenv('ONVIF_HOST', '192.168.1.100')
    config['port'] = int(os.getenv('ONVIF_PORT', '80'))
    config['username'] = os.getenv('ONVIF_USERNAME', 'admin')
    config['password'] = os.getenv('ONVIF_PASSWORD', 'password')
    config['log_level'] = os.getenv('LOG_LEVEL', 'INFO')
    
    return config


def main():
    """Main example function"""
    
    # Load configuration
    config = load_config()
    
    # Setup logging
    import logging
    log_levels = {
        'DEBUG': logging.DEBUG,
        'INFO': logging.INFO,
        'WARNING': logging.WARNING,
        'ERROR': logging.ERROR,
        'CRITICAL': logging.CRITICAL
    }
    log_level = log_levels.get(config.get('log_level', 'INFO'), logging.INFO)
    setup_logging(level=log_level)
    
    print("=" * 60)
    print("ONVIF Camera Client Example")
    print("=" * 60)
    print()
    
    # Create ONVIF client instance
    print(f"Connecting to camera at {config['host']}:{config['port']}...")
    client = ONVIFClient(
        config['host'],
        config['port'],
        config['username'],
        config['password']
    )
    
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
