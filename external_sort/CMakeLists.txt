add_compile_options(-std=c++14 -Wfatal-errors)
include_directories("include/")

file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(external_sort ${HEADERS} ${SOURCES})

add_subdirectory(tests)

add_test(NAME test_sort COMMAND test_sort
         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests")

enable_testing()

add_subdirectory(bin)