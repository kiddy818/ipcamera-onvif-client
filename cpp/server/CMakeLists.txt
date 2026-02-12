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
            include_directories(${GSOAP_INCLUDE_DIRS})
            link_directories(${GSOAP_LIBRARY_DIRS})
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

if(ENABLE_CUCUMBER_TEST)
    find_package(GTest 1.11.0 REQUIRED)
endif()
if(TARGET GTest::gtest)
    find_package(CucumberCpp REQUIRED)
    function(add_cucumber_executable)
        add_executable(FeatureShowcaseSteps ${ARGV})
        target_include_directories(FeatureShowcaseSteps PUBLIC ${CucumberCpp_DIR}/../../include)
        target_link_directories(FeatureShowcaseSteps PUBLIC ${CucumberCpp_DIR}/../)
        target_link_libraries(FeatureShowcaseSteps PRIVATE onvif_server cucumber-cpp GTest::gtest)
        foreach(_arg ${ARGN})
            get_filename_component(OBJECT_PREFIX ${_arg} NAME_WE)
            set_source_files_properties(${_arg} PROPERTIES COMPILE_FLAGS "-DCUKE_OBJECT_PREFIX=${OBJECT_PREFIX}")
        endforeach(_arg)
    endfunction()

    add_cucumber_executable(
        features/step_definitions/server_device_management.cpp
    )
endif()

install(DIRECTORY include/
    DESTINATION include/onvif_server
    FILES_MATCHING PATTERN "*.h"
)
