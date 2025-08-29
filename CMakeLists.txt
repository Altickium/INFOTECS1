cmake_minimum_required(VERSION 3.10)
project(infotecs-test-task)

set(CMAKE_CXX_STANDARD 17)

add_library(journal-writer-lib SHARED src/journal_writer.cpp)
target_include_directories(journal-writer-lib PUBLIC include)

add_executable(main-app src/main.cpp)
target_link_libraries(main-app journal-writer-lib)