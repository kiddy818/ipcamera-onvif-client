Feature: ONVIF Server Device Management Service
  As an ONVIF server
  I want to handle device management requests from clients
  So that clients can discover device information and capabilities

  Scenario: Server initialization
    Given the ONVIF server is configured on port 8080
    When I start the ONVIF server
    Then the server should be running successfully
    And the server should be listening on the configured port

  Scenario: Handle GetDeviceInformation request
    Given the ONVIF server is running
    When a client requests device information
    Then the server should return manufacturer information
    And the server should return model information
    And the server should return firmware version information
    And the server should return serial number information
    And the response should be valid SOAP format

  Scenario: Handle GetCapabilities request
    Given the ONVIF server is running
    When a client requests device capabilities
    Then the server should return Device service capability
    And the server should return Media service capability
    And the server should return service URLs
    And the response should be valid SOAP format

  Scenario: Handle GetServices request
    Given the ONVIF server is running
    When a client requests available services
    Then the server should return a list of available services
    And each service should have a namespace
    And each service should have an XAddr URL
    And the response should be valid SOAP format

  Scenario: Handle GetSystemDateAndTime request
    Given the ONVIF server is running
    When a client requests system date and time
    Then the server should return current date and time
    And the time should be in UTC format
    And the response should be valid SOAP format

  Scenario: Handle authenticated request
    Given the ONVIF server requires authentication
    And valid credentials "admin" and "password" are configured
    When a client sends an authenticated request
    Then the server should validate WS-UsernameToken
    And the server should accept the request
    And the server should return successful response

  Scenario: Reject unauthenticated request
    Given the ONVIF server requires authentication
    When a client sends a request without credentials
    Then the server should reject the request
    And the server should return SOAP fault
    And the fault should indicate authentication failure

  Scenario: Reject invalid credentials
    Given the ONVIF server requires authentication
    When a client sends a request with invalid credentials
    Then the server should reject the request
    And the server should return SOAP fault
    And the fault should indicate authentication failure
