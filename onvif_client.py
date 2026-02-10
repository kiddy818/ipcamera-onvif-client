#!/usr/bin/env python3
"""
ONVIF Client for IP Cameras
A basic implementation of an ONVIF client to interact with IP cameras
"""

import logging
from typing import Optional, List, Dict, Any
from onvif import ONVIFCamera
from zeep.exceptions import Fault


class ONVIFClient:
    """Basic ONVIF Client for IP Camera operations"""
    
    def __init__(self, host: str, port: int, username: str, password: str):
        """
        Initialize ONVIF client
        
        Args:
            host: IP address or hostname of the camera
            port: ONVIF service port (usually 80, 8080, or 8000)
            username: Camera username
            password: Camera password
        """
        self.host = host
        self.port = port
        self.username = username
        self.password = password
        self.camera: Optional[ONVIFCamera] = None
        self.media_service = None
        self.ptz_service = None
        self.device_service = None
        
        # Setup logging
        self.logger = logging.getLogger(__name__)
        
    def connect(self) -> bool:
        """
        Connect to the ONVIF camera
        
        Returns:
            bool: True if connection successful, False otherwise
        """
        try:
            self.logger.info(f"Connecting to camera at {self.host}:{self.port}")
            self.camera = ONVIFCamera(
                self.host, 
                self.port, 
                self.username, 
                self.password
            )
            
            # Initialize services
            self.device_service = self.camera.create_devicemgmt_service()
            self.media_service = self.camera.create_media_service()
            
            # Try to initialize PTZ service (may not be available on all cameras)
            try:
                self.ptz_service = self.camera.create_ptz_service()
            except Exception as e:
                self.logger.warning(f"PTZ service not available: {e}")
                self.ptz_service = None
            
            self.logger.info("Successfully connected to camera")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to connect to camera: {e}")
            return False
    
    def get_device_information(self) -> Optional[Dict[str, Any]]:
        """
        Get device information from the camera
        
        Returns:
            dict: Device information including manufacturer, model, firmware version, etc.
        """
        try:
            if not self.device_service:
                self.logger.error("Device service not initialized")
                return None
                
            info = self.device_service.GetDeviceInformation()
            
            device_info = {
                'Manufacturer': info.Manufacturer,
                'Model': info.Model,
                'FirmwareVersion': info.FirmwareVersion,
                'SerialNumber': info.SerialNumber,
                'HardwareId': info.HardwareId
            }
            
            self.logger.info(f"Device info retrieved: {device_info['Manufacturer']} {device_info['Model']}")
            return device_info
            
        except Fault as e:
            self.logger.error(f"SOAP Fault while getting device info: {e}")
            return None
        except Exception as e:
            self.logger.error(f"Error getting device information: {e}")
            return None
    
    def get_profiles(self) -> Optional[List[Any]]:
        """
        Get media profiles from the camera
        
        Returns:
            list: List of media profiles
        """
        try:
            if not self.media_service:
                self.logger.error("Media service not initialized")
                return None
                
            profiles = self.media_service.GetProfiles()
            self.logger.info(f"Retrieved {len(profiles)} media profiles")
            return profiles
            
        except Fault as e:
            self.logger.error(f"SOAP Fault while getting profiles: {e}")
            return None
        except Exception as e:
            self.logger.error(f"Error getting profiles: {e}")
            return None
    
    def get_stream_uri(self, profile_token: Optional[str] = None) -> Optional[str]:
        """
        Get the RTSP stream URI for a profile
        
        Args:
            profile_token: Token of the profile to get stream URI for.
                          If None, uses the first available profile.
        
        Returns:
            str: RTSP stream URI
        """
        try:
            if not self.media_service:
                self.logger.error("Media service not initialized")
                return None
            
            # If no profile token provided, get the first profile
            if profile_token is None:
                profiles = self.get_profiles()
                if not profiles:
                    self.logger.error("No profiles available")
                    return None
                profile_token = profiles[0].token
            
            # Create stream setup
            stream_setup = self.media_service.create_type('GetStreamUri')
            stream_setup.ProfileToken = profile_token
            stream_setup.StreamSetup = {
                'Stream': 'RTP-Unicast',
                'Transport': {'Protocol': 'RTSP'}
            }
            
            stream_uri = self.media_service.GetStreamUri(stream_setup)
            uri = stream_uri.Uri
            
            self.logger.info(f"Stream URI retrieved: {uri}")
            return uri
            
        except Fault as e:
            self.logger.error(f"SOAP Fault while getting stream URI: {e}")
            return None
        except Exception as e:
            self.logger.error(f"Error getting stream URI: {e}")
            return None
    
    def get_snapshot_uri(self, profile_token: Optional[str] = None) -> Optional[str]:
        """
        Get the snapshot URI for a profile
        
        Args:
            profile_token: Token of the profile to get snapshot URI for.
                          If None, uses the first available profile.
        
        Returns:
            str: Snapshot URI
        """
        try:
            if not self.media_service:
                self.logger.error("Media service not initialized")
                return None
            
            # If no profile token provided, get the first profile
            if profile_token is None:
                profiles = self.get_profiles()
                if not profiles:
                    self.logger.error("No profiles available")
                    return None
                profile_token = profiles[0].token
            
            # Get snapshot URI
            request = self.media_service.create_type('GetSnapshotUri')
            request.ProfileToken = profile_token
            
            snapshot_uri = self.media_service.GetSnapshotUri(request)
            uri = snapshot_uri.Uri
            
            self.logger.info(f"Snapshot URI retrieved: {uri}")
            return uri
            
        except Fault as e:
            self.logger.error(f"SOAP Fault while getting snapshot URI: {e}")
            return None
        except Exception as e:
            self.logger.error(f"Error getting snapshot URI: {e}")
            return None
    
    def move_ptz(self, pan: float, tilt: float, zoom: float, 
                 profile_token: Optional[str] = None) -> bool:
        """
        Move PTZ (Pan-Tilt-Zoom) camera
        
        Args:
            pan: Pan value (-1.0 to 1.0, negative is left, positive is right)
            tilt: Tilt value (-1.0 to 1.0, negative is down, positive is up)
            zoom: Zoom value (-1.0 to 1.0, negative is zoom out, positive is zoom in)
            profile_token: Profile token to use. If None, uses the first available profile.
        
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if not self.ptz_service:
                self.logger.error("PTZ service not available")
                return False
            
            # If no profile token provided, get the first profile
            if profile_token is None:
                profiles = self.get_profiles()
                if not profiles:
                    self.logger.error("No profiles available")
                    return False
                profile_token = profiles[0].token
            
            # Create continuous move request
            request = self.ptz_service.create_type('ContinuousMove')
            request.ProfileToken = profile_token
            request.Velocity = {
                'PanTilt': {'x': pan, 'y': tilt},
                'Zoom': {'x': zoom}
            }
            
            self.ptz_service.ContinuousMove(request)
            self.logger.info(f"PTZ moved: pan={pan}, tilt={tilt}, zoom={zoom}")
            return True
            
        except Fault as e:
            self.logger.error(f"SOAP Fault while moving PTZ: {e}")
            return False
        except Exception as e:
            self.logger.error(f"Error moving PTZ: {e}")
            return False
    
    def stop_ptz(self, profile_token: Optional[str] = None) -> bool:
        """
        Stop PTZ movement
        
        Args:
            profile_token: Profile token to use. If None, uses the first available profile.
        
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if not self.ptz_service:
                self.logger.error("PTZ service not available")
                return False
            
            # If no profile token provided, get the first profile
            if profile_token is None:
                profiles = self.get_profiles()
                if not profiles:
                    self.logger.error("No profiles available")
                    return False
                profile_token = profiles[0].token
            
            # Stop PTZ
            request = self.ptz_service.create_type('Stop')
            request.ProfileToken = profile_token
            request.PanTilt = True
            request.Zoom = True
            
            self.ptz_service.Stop(request)
            self.logger.info("PTZ stopped")
            return True
            
        except Fault as e:
            self.logger.error(f"SOAP Fault while stopping PTZ: {e}")
            return False
        except Exception as e:
            self.logger.error(f"Error stopping PTZ: {e}")
            return False
    
    def get_capabilities(self) -> Optional[Dict[str, Any]]:
        """
        Get device capabilities
        
        Returns:
            dict: Device capabilities
        """
        try:
            if not self.device_service:
                self.logger.error("Device service not initialized")
                return None
            
            capabilities = self.device_service.GetCapabilities()
            
            caps_dict = {
                'Analytics': capabilities.Analytics is not None,
                'Device': capabilities.Device is not None,
                'Events': capabilities.Events is not None,
                'Imaging': capabilities.Imaging is not None,
                'Media': capabilities.Media is not None,
                'PTZ': capabilities.PTZ is not None
            }
            
            self.logger.info(f"Device capabilities retrieved: {caps_dict}")
            return caps_dict
            
        except Fault as e:
            self.logger.error(f"SOAP Fault while getting capabilities: {e}")
            return None
        except Exception as e:
            self.logger.error(f"Error getting capabilities: {e}")
            return None


def setup_logging(level=logging.INFO):
    """Setup logging configuration"""
    logging.basicConfig(
        level=level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
