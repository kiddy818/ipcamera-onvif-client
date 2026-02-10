cmake_minimum_required(VERSION 3.10)

# BDD Test Runner
add_executable(test_cucumber_runner cucumber_runner.c)
target_link_libraries(test_cucumber_runner onvif_server)

# Add test
add_test(NAME ServerBDDTests COMMAND test_cucumber_runner)
