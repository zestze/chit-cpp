cmake_minimum_required(VERSION 3.10)
project(chit_cpp VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_subdirectory(src)



if(FALSE) # fake a block comment

include_directories(src/client)
include_directories(src/shared)
include_directories(src/server)

#enable_testing()
add_executable(simpleClientTester
        test/UserTester.cpp)
target_link_libraries(simpleClientTester gtest gtest_main Threads::Threads)

endif()
