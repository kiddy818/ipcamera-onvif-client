# Development Guide

## Development Environment Setup

### Prerequisites

- C compiler (GCC 7+ or Clang 6+)
- CMake 3.10+
- libcurl with OpenSSL
- Git
- Text editor or IDE

### Quick Start

```bash
# Clone repository
git clone https://github.com/kiddy818/ipcamera-onvif-client.git
cd ipcamera-onvif-client

# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake libcurl4-openssl-dev

# Build
mkdir build && cd build
cmake ..
make

# Run tests
ctest --verbose

# Run example
./examples/onvif_example http://camera-ip username password
```

## Project Structure

```
ipcamera-onvif-client/
├── include/              # Public API headers
│   ├── onvif_client.h   # Main client API
│   ├── soap_handler.h   # SOAP processing
│   └── http_client.h    # HTTP communication
├── src/                  # Implementation
│   ├── onvif_client.c   # Client initialization
│   ├── device_service.c # Device Management Service
│   ├── media_service.c  # Media Service
│   ├── ptz_service.c    # PTZ Service
│   ├── soap_handler.c   # SOAP message handling
│   ├── http_client.c    # HTTP/CURL wrapper
│   └── auth.c           # Authentication
├── tests/                # Test suite
│   ├── test_onvif_client.c      # Client tests
│   ├── test_device_service.c    # Device service tests
│   ├── test_media_service.c     # Media service tests
│   └── cucumber_runner.c        # BDD test runner
├── features/             # BDD feature files (Gherkin)
│   ├── device_management.feature
│   ├── media_service.feature
│   ├── ptz_service.feature
│   └── authentication.feature
├── examples/             # Example applications
│   └── onvif_example.c
├── CMakeLists.txt        # Build configuration
└── README.md             # Project documentation
```

## Development Workflow

### 1. Requirements-Driven Development

We follow BDD principles:

1. **Write Feature**: Define requirement in Gherkin
   ```gherkin
   Feature: New Feature
     Scenario: Do something
       Given initial state
       When action occurs
       Then expected result
   ```

2. **Implement Test**: Add test case
   ```c
   int test_new_feature() {
       // Test implementation
       return 0;
   }
   ```

3. **Write Code**: Implement feature
   ```c
   int onvif_new_feature(onvif_client_t* client) {
       // Implementation
       return ONVIF_OK;
   }
   ```

4. **Run Tests**: Verify it works
   ```bash
   cd build
   make
   ctest
   ```

### 2. Coding Standards

#### Style Guidelines

- **Indentation**: 4 spaces (no tabs)
- **Line length**: 100 characters max
- **Braces**: K&R style (opening brace on same line)
- **Naming**:
  - Functions: `onvif_service_action()` (snake_case)
  - Types: `onvif_type_t` (snake_case with _t suffix)
  - Macros: `ONVIF_CONSTANT` (UPPER_CASE)
  - Private functions: `static` with same naming

#### Example Code Style

```c
/* Good */
int onvif_get_device_information(onvif_client_t* client, 
                                 onvif_device_info_t* info) {
    if (!client || !info) {
        return ONVIF_ERR_INVALID_PARAM;
    }
    
    /* Implementation */
    
    return ONVIF_OK;
}

/* Bad */
int GetDeviceInformation( onvif_client_t* client , onvif_device_info_t* info )
{
  if(!client||!info)return ONVIF_ERR_INVALID_PARAM;
  // Implementation
  return ONVIF_OK;
}
```

#### Error Handling

Always validate parameters and return appropriate error codes:

```c
int onvif_function(onvif_client_t* client, void* data) {
    /* Validate parameters */
    if (!client || !data) {
        return ONVIF_ERR_INVALID_PARAM;
    }
    
    /* Check state */
    if (client->state != READY) {
        return ONVIF_ERR_NOT_READY;
    }
    
    /* Perform operation */
    int ret = do_operation();
    if (ret != 0) {
        return ONVIF_ERR_NETWORK;
    }
    
    return ONVIF_OK;
}
```

### 3. Testing

#### Running Tests

```bash
# All tests
cd build
ctest --verbose

# Specific test
./tests/test_onvif_client
./tests/test_device_service
./tests/test_media_service
./tests/test_cucumber_runner

# With output
ctest --output-on-failure
```

#### Writing Unit Tests

Add tests to appropriate test file:

```c
TEST(test_new_function) {
    onvif_client_t client;
    onvif_client_init(&client, "http://test", "user", "pass");
    
    int ret = onvif_new_function(&client);
    assert(ret == ONVIF_OK);
    
    onvif_client_destroy(&client);
}

int main() {
    RUN_TEST(test_new_function);
    return 0;
}
```

#### Writing BDD Tests

Add scenario to `tests/cucumber_runner.c`:

```c
int test_new_scenario() {
    printf("    Given initial condition\n");
    printf("    When action is performed\n");
    printf("    Then expected result occurs\n");
    
    /* Test implementation */
    
    printf("    ✓ PASSED\n");
    return 0;
}

/* Add to bdd_tests array */
bdd_test_t bdd_tests[] = {
    {"Feature Name", "Scenario Name", test_new_scenario},
    /* ... */
    {NULL, NULL, NULL}
};
```

### 4. Building

#### Debug Build

```bash
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

#### Release Build

```bash
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

#### With Address Sanitizer (for memory debugging)

```bash
mkdir build-asan && cd build-asan
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_C_FLAGS="-fsanitize=address -g" \
      -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address" ..
make
```

### 5. Debugging

#### Using GDB

```bash
cd build
gdb ./examples/onvif_example

(gdb) break onvif_get_device_information
(gdb) run http://192.168.1.100 admin password
(gdb) step
(gdb) print client->device_url
```

#### Verbose Logging

Enable curl verbose mode in `src/http_client.c`:

```c
curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
```

Add debug prints:

```c
#ifdef DEBUG
    printf("DEBUG: Sending request to %s\n", url);
    printf("DEBUG: Body: %s\n", data);
#endif
```

Build with debug flag:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DDEBUG=1 ..
```

### 6. Adding New Features

#### Step-by-Step Example: Add GetSystemDateAndTime

1. **Add feature description** to `features/device_management.feature`:

```gherkin
Scenario: Get system date and time
  Given an initialized ONVIF client
  When I request system date and time
  Then I should receive the current date and time
```

2. **Add structure** to `include/onvif_client.h`:

```c
typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} onvif_date_time_t;

int onvif_get_system_date_time(onvif_client_t* client, 
                                onvif_date_time_t* dt);
```

3. **Implement function** in `src/device_service.c`:

```c
int onvif_get_system_date_time(onvif_client_t* client,
                                onvif_date_time_t* dt) {
    if (!client || !dt) {
        return ONVIF_ERR_INVALID_PARAM;
    }
    
    /* Construct SOAP request */
    const char* body = 
        "<GetSystemDateAndTime xmlns=\"http://www.onvif.org/ver10/device/wsdl\"/>";
    
    /* Send request and parse response */
    /* ... implementation ... */
    
    return ONVIF_OK;
}
```

4. **Add test** in `tests/test_device_service.c`:

```c
TEST(test_get_system_date_time) {
    onvif_client_t client;
    onvif_date_time_t dt;
    
    onvif_client_init(&client, "http://test", "user", "pass");
    
    int ret = onvif_get_system_date_time(&client, &dt);
    /* Without actual device, expect network error */
    /* assert(ret == ONVIF_ERR_NETWORK || ret == ONVIF_OK); */
    
    onvif_client_destroy(&client);
}
```

5. **Build and test**:

```bash
cd build
cmake ..
make
ctest
```

## Contributing

### Pull Request Process

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Write tests first (TDD/BDD)
4. Implement feature
5. Ensure all tests pass
6. Update documentation
7. Submit pull request

### PR Checklist

- [ ] Feature defined in Gherkin (if user-facing)
- [ ] Tests added and passing
- [ ] Code follows style guidelines
- [ ] Error handling implemented
- [ ] Documentation updated
- [ ] Example updated (if API changed)
- [ ] No compiler warnings
- [ ] Memory leaks checked (valgrind)

## Testing with Real Devices

### ONVIF Test Camera

If you have an ONVIF camera:

```bash
# Discover camera IP (if unknown)
# Use ONVIF Device Manager or similar tool

# Test with real device
./build/examples/onvif_example http://192.168.1.100 admin password123
```

### ONVIF Simulator

For development without hardware:

```bash
# Use happytimesoft ONVIF server or similar
# Or Docker-based ONVIF simulator
docker run -p 8080:8080 onvif-simulator
```

### Wireshark Analysis

Capture and analyze ONVIF traffic:

```bash
# Capture on network interface
sudo wireshark -i eth0 -f "port 80 or port 8080"

# Filter SOAP messages
http.content_type contains "soap+xml"
```

## Performance Considerations

### Memory Management

- Always free allocated memory
- Use valgrind to check leaks:
  ```bash
  valgrind --leak-check=full ./examples/onvif_example http://camera user pass
  ```

### Network Optimization

- Reuse connections when possible
- Set appropriate timeouts
- Handle partial responses

### Concurrency

Current implementation is single-threaded. For concurrent requests:
- Create separate client instances per thread
- Add mutex protection if sharing clients

## Documentation

### Updating Documentation

When changing API:
1. Update header file comments
2. Update README.md usage section
3. Update IMPLEMENTATION_GUIDE.md if architectural
4. Update this file if process changes

### Comment Style

```c
/**
 * Brief description of function.
 * 
 * Detailed description if needed.
 * 
 * @param client Pointer to initialized client
 * @param info   Pointer to store device info
 * @return ONVIF_OK on success, error code otherwise
 */
int onvif_get_device_information(onvif_client_t* client,
                                 onvif_device_info_t* info);
```

## Release Process

1. Update version in CMakeLists.txt
2. Update CHANGELOG.md
3. Tag release: `git tag v1.0.0`
4. Build release binaries
5. Run full test suite
6. Create GitHub release

## Support

- GitHub Issues: Report bugs and request features
- Discussions: Ask questions and share ideas
- Wiki: Additional documentation and examples

## License

See LICENSE file for details.
