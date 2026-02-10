#!/usr/bin/env python3
"""
Simple tests for ONVIF Client
These are basic tests to validate the structure and imports
"""

import sys
import unittest
from unittest.mock import Mock, patch, MagicMock


class TestONVIFClientStructure(unittest.TestCase):
    """Test ONVIF Client class structure and basic functionality"""
    
    def test_import(self):
        """Test that the module can be imported"""
        try:
            import onvif_client
            self.assertIsNotNone(onvif_client)
        except ImportError as e:
            self.fail(f"Failed to import onvif_client: {e}")
    
    def test_class_exists(self):
        """Test that ONVIFClient class exists"""
        from onvif_client import ONVIFClient
        self.assertTrue(hasattr(ONVIFClient, '__init__'))
    
    def test_initialization(self):
        """Test that ONVIFClient can be initialized"""
        from onvif_client import ONVIFClient
        
        client = ONVIFClient(
            host='192.168.1.100',
            port=80,
            username='admin',
            password='password'
        )
        
        self.assertEqual(client.host, '192.168.1.100')
        self.assertEqual(client.port, 80)
        self.assertEqual(client.username, 'admin')
        self.assertEqual(client.password, 'password')
    
    def test_methods_exist(self):
        """Test that all required methods exist"""
        from onvif_client import ONVIFClient
        
        client = ONVIFClient('192.168.1.100', 80, 'admin', 'password')
        
        # Check that methods exist
        self.assertTrue(hasattr(client, 'connect'))
        self.assertTrue(hasattr(client, 'get_device_information'))
        self.assertTrue(hasattr(client, 'get_profiles'))
        self.assertTrue(hasattr(client, 'get_stream_uri'))
        self.assertTrue(hasattr(client, 'get_snapshot_uri'))
        self.assertTrue(hasattr(client, 'move_ptz'))
        self.assertTrue(hasattr(client, 'stop_ptz'))
        self.assertTrue(hasattr(client, 'get_capabilities'))
    
    def test_methods_are_callable(self):
        """Test that all methods are callable"""
        from onvif_client import ONVIFClient
        
        client = ONVIFClient('192.168.1.100', 80, 'admin', 'password')
        
        self.assertTrue(callable(client.connect))
        self.assertTrue(callable(client.get_device_information))
        self.assertTrue(callable(client.get_profiles))
        self.assertTrue(callable(client.get_stream_uri))
        self.assertTrue(callable(client.get_snapshot_uri))
        self.assertTrue(callable(client.move_ptz))
        self.assertTrue(callable(client.stop_ptz))
        self.assertTrue(callable(client.get_capabilities))
    
    @patch('onvif_client.ONVIFCamera')
    def test_connect_creates_services(self, mock_onvif_camera):
        """Test that connect initializes the required services"""
        from onvif_client import ONVIFClient
        
        # Setup mock
        mock_camera_instance = MagicMock()
        mock_onvif_camera.return_value = mock_camera_instance
        
        # Create client and connect
        client = ONVIFClient('192.168.1.100', 80, 'admin', 'password')
        result = client.connect()
        
        # Verify connection was attempted
        mock_onvif_camera.assert_called_once_with('192.168.1.100', 80, 'admin', 'password')
        self.assertTrue(result)
        
        # Verify services were created
        mock_camera_instance.create_devicemgmt_service.assert_called_once()
        mock_camera_instance.create_media_service.assert_called_once()
    
    def test_setup_logging_function(self):
        """Test that setup_logging function exists and is callable"""
        from onvif_client import setup_logging
        
        self.assertTrue(callable(setup_logging))
        
        # Test that it can be called without errors
        try:
            setup_logging()
        except Exception as e:
            self.fail(f"setup_logging() raised an exception: {e}")


class TestExampleScript(unittest.TestCase):
    """Test that the example script is properly structured"""
    
    def test_example_imports(self):
        """Test that example.py can be imported"""
        try:
            import example
            self.assertIsNotNone(example)
        except ImportError as e:
            self.fail(f"Failed to import example: {e}")
    
    def test_example_has_main(self):
        """Test that example has a main function"""
        import example
        self.assertTrue(hasattr(example, 'main'))
        self.assertTrue(callable(example.main))


if __name__ == '__main__':
    print("Running ONVIF Client structure tests...")
    print("=" * 60)
    
    # Run tests
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromModule(sys.modules[__name__])
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Print summary
    print()
    print("=" * 60)
    if result.wasSuccessful():
        print("✓ All tests passed!")
        sys.exit(0)
    else:
        print("✗ Some tests failed")
        sys.exit(1)
