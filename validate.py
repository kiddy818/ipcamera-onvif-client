#!/usr/bin/env python3
"""
Validation script for ONVIF Client
This script validates the API surface and structure without requiring an actual camera
"""

import sys
from onvif_client import ONVIFClient, setup_logging


def validate_api():
    """Validate the ONVIF client API"""
    
    print("=" * 60)
    print("ONVIF Client API Validation")
    print("=" * 60)
    print()
    
    # Create a client instance (no connection required for this test)
    print("✓ Creating ONVIFClient instance...")
    client = ONVIFClient('192.168.1.100', 80, 'admin', 'password')
    
    # Validate attributes
    print("✓ Validating client attributes...")
    assert client.host == '192.168.1.100'
    assert client.port == 80
    assert client.username == 'admin'
    assert client.password == 'password'
    
    # Validate methods exist
    print("✓ Validating client methods...")
    methods = [
        'connect',
        'get_device_information',
        'get_profiles',
        'get_stream_uri',
        'get_snapshot_uri',
        'move_ptz',
        'stop_ptz',
        'get_capabilities'
    ]
    
    for method in methods:
        assert hasattr(client, method), f"Missing method: {method}"
        assert callable(getattr(client, method)), f"Method not callable: {method}"
    
    print(f"  - All {len(methods)} methods present and callable")
    
    # Validate logging function
    print("✓ Validating setup_logging function...")
    setup_logging()
    
    print()
    print("=" * 60)
    print("✓ All API validations passed!")
    print("=" * 60)
    print()
    print("Summary:")
    print("  - ONVIFClient class: OK")
    print("  - Client initialization: OK")
    print(f"  - Public methods: {len(methods)} methods OK")
    print("  - Logging setup: OK")
    print()
    print("The ONVIF client is ready to use!")
    print()
    print("Next steps:")
    print("  1. Configure your camera credentials in config.ini")
    print("  2. Run: python example.py")
    print("  3. Or use the client in your own Python scripts")
    
    return True


if __name__ == '__main__':
    try:
        if validate_api():
            sys.exit(0)
        else:
            sys.exit(1)
    except Exception as e:
        print(f"✗ Validation failed: {e}")
        sys.exit(1)
