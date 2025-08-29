cmake_minimum_required(VERSION 3.10)
project(infotecs-test-task)

set(CMAKE_CXX_STANDARD 17)

add_library(main-logger SHARED src/logger.cpp)
target_include_directories(main-logger PUBLIC include)

add_executable(main-app src/main.cpp)
target_link_libraries(main-app main-logger)