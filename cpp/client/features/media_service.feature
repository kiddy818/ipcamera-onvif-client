Feature: ONVIF Media Service
  As an ONVIF client
  I want to retrieve media profiles and streaming URIs
  So that I can access video streams from the IP camera

  Scenario: Get media profiles
    Given an initialized ONVIF client
    When I request media profiles
    Then I should receive at least one profile
    And each profile should have a token
    And each profile should have a name

  Scenario: Get stream URI for a profile
    Given an initialized ONVIF client
    And a valid media profile token
    When I request the stream URI for the profile
    Then I should receive a valid RTSP URI
    And the URI should be accessible

  Scenario: Get snapshot URI for a profile
    Given an initialized ONVIF client
    And a valid media profile token
    When I request the snapshot URI for the profile
    Then I should receive a valid HTTP URI
    And the URI should point to a JPEG image

  Scenario: Handle invalid profile token
    Given an initialized ONVIF client
    When I request the stream URI with an invalid token
    Then I should receive an error response
