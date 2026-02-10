Feature: ONVIF PTZ Service
  As an ONVIF client
  I want to control PTZ (Pan-Tilt-Zoom) functionality
  So that I can control the camera position and zoom

  Scenario: Get PTZ configurations
    Given an initialized ONVIF client with PTZ support
    When I request PTZ configurations
    Then I should receive PTZ configuration information

  Scenario: PTZ not supported
    Given an initialized ONVIF client without PTZ support
    When I request PTZ configurations
    Then I should receive a not supported error
