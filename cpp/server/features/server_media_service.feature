Feature: ONVIF Server Media Service
  As an ONVIF server
  I want to handle media service requests from clients
  So that clients can retrieve media profiles and stream URIs

  Scenario: Handle GetProfiles request
    Given the ONVIF server is running
    And media profiles are configured
    When a client requests media profiles
    Then the server should return a list of media profiles
    And each profile should have a unique token
    And each profile should have a name
    And each profile should have video encoder configuration
    And the response should be valid SOAP format

  Scenario: Handle GetStreamUri request
    Given the ONVIF server is running
    And a media profile "profile_1" exists
    When a client requests stream URI for profile "profile_1"
    Then the server should return an RTSP stream URI
    And the URI should include the profile token
    And the URI should be properly formatted
    And the response should be valid SOAP format

  Scenario: Handle GetSnapshotUri request
    Given the ONVIF server is running
    And a media profile "profile_1" exists
    When a client requests snapshot URI for profile "profile_1"
    Then the server should return an HTTP snapshot URI
    And the URI should include the profile token
    And the URI should be properly formatted
    And the response should be valid SOAP format

  Scenario: Handle GetVideoEncoderConfiguration request
    Given the ONVIF server is running
    And a video encoder configuration exists
    When a client requests video encoder configuration
    Then the server should return encoder configuration
    And the configuration should include encoding type
    And the configuration should include resolution
    And the configuration should include frame rate
    And the response should be valid SOAP format

  Scenario: Handle invalid profile token
    Given the ONVIF server is running
    When a client requests stream URI with invalid token "invalid_token"
    Then the server should return SOAP fault
    And the fault should indicate invalid token
    And the fault code should be appropriate

  Scenario: Handle multiple profiles
    Given the ONVIF server is running
    And multiple media profiles are configured
    When a client requests media profiles
    Then the server should return all available profiles
    And each profile should be uniquely identifiable
    And profiles should be in correct format
