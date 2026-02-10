cmake_minimum_required(VERSION 3.10)

# Unit tests - separate executables
add_executable(test_onvif_client
    test_onvif_client.c
)

target_include_directories(test_onvif_client PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(test_onvif_client onvif_client)

add_test(NAME test_onvif_client COMMAND test_onvif_client)

# Device service tests
add_executable(test_device_service
    test_device_service.c
)

target_include_directories(test_device_service PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(test_device_service onvif_client)

add_test(NAME test_device_service COMMAND test_device_service)

# Media service tests
add_executable(test_media_service
    test_media_service.c
)

target_include_directories(test_media_service PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(test_media_service onvif_client)

add_test(NAME test_media_service COMMAND test_media_service)

# BDD tests with CWT-Cucumber (would require cwt-cucumber installation)
# For now, we'll create placeholder test runner
add_executable(test_cucumber_runner
    cucumber_runner.c
)

target_include_directories(test_cucumber_runner PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(test_cucumber_runner onvif_client)

add_test(NAME test_cucumber_runner COMMAND test_cucumber_runner)
