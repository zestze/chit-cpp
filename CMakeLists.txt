cmake_minimum_required(VERSION 3.10)
project(chit_cpp VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/src/shared")

add_subdirectory(src)

add_subdirectory(test)
