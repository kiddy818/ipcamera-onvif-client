Feature: ONVIF Server SOAP Message Handling
  As an ONVIF server
  I want to properly handle SOAP requests and responses
  So that clients can communicate using ONVIF protocol

  Scenario: Parse valid SOAP request
    Given the ONVIF server is running
    When a client sends a valid SOAP request
    Then the server should parse the SOAP envelope
    And the server should extract the SOAP header
    And the server should extract the SOAP body
    And the server should identify the requested operation

  Scenario: Return SOAP response
    Given the ONVIF server processed a request
    When generating a response
    Then the server should create valid SOAP envelope
    And the envelope should use correct namespaces
    And the body should contain the response data
    And the response should be properly formatted XML

  Scenario: Return SOAP fault for invalid request
    Given the ONVIF server receives invalid SOAP request
    When processing the request fails
    Then the server should return SOAP fault
    And the fault should have appropriate fault code
    And the fault should have descriptive fault string
    And the fault should be valid SOAP format

  Scenario: Handle malformed XML
    Given the ONVIF server is running
    When a client sends malformed XML
    Then the server should detect the parse error
    And the server should return SOAP fault
    And the fault code should indicate malformed request

  Scenario: Handle unknown SOAP action
    Given the ONVIF server is running
    When a client requests unknown operation
    Then the server should return SOAP fault
    And the fault should indicate unsupported action

  Scenario: Validate SOAP namespace
    Given the ONVIF server is running
    When a client sends request with correct namespaces
    Then the server should accept the request
    And process it normally

  Scenario: Support SOAP 1.2
    Given the ONVIF server supports SOAP 1.2
    When a client sends SOAP 1.2 formatted request
    Then the server should parse it correctly
    And return SOAP 1.2 formatted response
