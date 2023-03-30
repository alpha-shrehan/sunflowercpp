include_directories(../)
# Test for sunflower
add_executable(sunflower_test_exe 01.cpp)
target_link_libraries(sunflower_test_exe sunflower)
add_test(sunflower_test sunflower_test_exe)