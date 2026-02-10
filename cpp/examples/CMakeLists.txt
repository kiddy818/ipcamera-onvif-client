cmake_minimum_required(VERSION 3.10)

add_executable(onvif_example
    onvif_example.c
)

target_include_directories(onvif_example PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(onvif_example onvif_client)
