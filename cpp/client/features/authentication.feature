Feature: ONVIF Authentication
  As an ONVIF client
  I want to authenticate with the device
  So that I can access protected services

  Scenario: Successful authentication
    Given an ONVIF device at URL "http://192.168.1.100"
    And valid credentials "admin" and "password"
    When I authenticate with the device
    Then authentication should succeed

  Scenario: Failed authentication with invalid credentials
    Given an ONVIF device at URL "http://192.168.1.100"
    And invalid credentials "wrong" and "credentials"
    When I authenticate with the device
    Then authentication should fail

  Scenario: Anonymous access
    Given an ONVIF device at URL "http://192.168.1.100"
    And no credentials
    When I attempt to access device information
    Then I should receive an authentication required error
