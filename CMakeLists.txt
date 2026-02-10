cmake_minimum_required(VERSION 3.10)
project(onvif_client VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(ONVIF_SOURCES
    src/onvif_client.c
    src/device_service.c
    src/media_service.c
    src/ptz_service.c
    src/soap_handler.c
    src/http_client.c
    src/auth.c
)

# Create library
add_library(onvif_client STATIC ${ONVIF_SOURCES})

# Link libraries
find_package(CURL REQUIRED)
target_link_libraries(onvif_client CURL::libcurl)

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
