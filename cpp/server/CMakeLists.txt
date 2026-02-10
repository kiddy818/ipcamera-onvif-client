cmake_minimum_required(VERSION 3.10)
project(onvif_server VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(USE_GSOAP "Use gSOAP for SOAP handling" OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(ONVIF_SERVER_SOURCES
    src/onvif_server.c
    src/device_service_handler.c
    src/media_service_handler.c
    src/soap_server_handler.c
    src/auth_handler.c
)

# Create library
add_library(onvif_server STATIC ${ONVIF_SERVER_SOURCES})

# Find OpenSSL (required for authentication)
find_package(OpenSSL REQUIRED)
target_link_libraries(onvif_server OpenSSL::SSL OpenSSL::Crypto)

# gSOAP support (optional)
if(USE_GSOAP)
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(GSOAP gsoap++)
        if(GSOAP_FOUND)
            message(STATUS "Found gSOAP: ${GSOAP_VERSION}")
            include_directories(${GSOAP_INCLUDE_DIRS})
            target_link_libraries(onvif_server ${GSOAP_LIBRARIES})
            target_compile_definitions(onvif_server PRIVATE USE_GSOAP)
        else()
            message(WARNING "gSOAP not found, using basic SOAP implementation")
        endif()
    endif()
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install targets
install(TARGETS onvif_server
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include/onvif_server
    FILES_MATCHING PATTERN "*.h"
)
