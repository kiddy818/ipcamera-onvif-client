Feature: ONVIF Device Management Service
  As an ONVIF client
  I want to communicate with an ONVIF-compliant IP camera
  So that I can retrieve device information and capabilities

  Scenario: Initialize ONVIF client
    Given an ONVIF device at URL "http://192.168.1.100"
    And valid credentials "admin" and "password"
    When I initialize the ONVIF client
    Then the client should be initialized successfully

  Scenario: Get device information
    Given an initialized ONVIF client
    When I request device information
    Then I should receive manufacturer information
    And I should receive model information
    And I should receive firmware version
    And I should receive serial number

  Scenario: Get device capabilities
    Given an initialized ONVIF client
    When I request device capabilities
    Then I should receive capabilities information
    And the device should support Device service
    And the device should support Media service

  Scenario: Get available services
    Given an initialized ONVIF client
    When I request available services
    Then I should receive a list of services
    And the list should contain device service URL
    And the list should contain media service URL
