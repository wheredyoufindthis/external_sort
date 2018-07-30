add_executable(test_sort main.cpp)

target_link_libraries(test_sort external_sort)
target_link_libraries(test_sort gtest_main)
