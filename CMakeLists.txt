cmake_minimum_required(VERSION 3.15)
project(log_analyzer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(loglib STATIC src/Parser.cpp src/Stats.cpp src/Analyzer.cpp src/Reporter.cpp)
target_include_directories(loglib PUBLIC src)

add_executable(log_analyzer src/main.cpp)
target_link_libraries(log_analyzer loglib)

enable_testing()
add_executable(log_tests tests/tests.cpp)
target_link_libraries(log_tests loglib)
add_test(NAME log_tests COMMAND log_tests)
