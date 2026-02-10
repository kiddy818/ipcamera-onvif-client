Feature: ONVIF Server Authentication
  As an ONVIF server
  I want to authenticate client requests
  So that only authorized clients can access the services

  Scenario: Validate WS-UsernameToken with password digest
    Given the ONVIF server requires authentication
    And valid credentials "admin" and "password" are configured
    When a client sends request with WS-UsernameToken
    And the token includes username "admin"
    And the token includes password digest
    And the token includes nonce
    And the token includes timestamp
    Then the server should validate the password digest
    And the server should accept the request

  Scenario: Reject expired timestamp
    Given the ONVIF server requires authentication
    When a client sends request with expired timestamp
    Then the server should reject the request
    And the server should return SOAP fault
    And the fault should indicate timestamp expired

  Scenario: Reject reused nonce
    Given the ONVIF server requires authentication
    And a nonce has been used recently
    When a client sends request with the same nonce
    Then the server should reject the request
    And the server should return SOAP fault
    And the fault should indicate replay attack

  Scenario: Support plain text password (optional)
    Given the ONVIF server allows plain text authentication
    When a client sends request with plain text password
    And the password matches configured credentials
    Then the server should accept the request

  Scenario: Multiple user authentication
    Given the ONVIF server has multiple users configured
    And user "admin" has password "admin123"
    And user "operator" has password "oper123"
    When a client authenticates as "operator"
    Then the server should validate against correct credentials
    And the server should accept the request
