cmake_minimum_required(VERSION 3.10)

# Server Example
add_executable(onvif_server_example server_example.c)
target_link_libraries(onvif_server_example onvif_server)
