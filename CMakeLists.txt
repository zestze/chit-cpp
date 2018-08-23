cmake_minimum_required(VERSION 3.10)
project(chit_cpp VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
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

# added to conforrm with random reddit omment.
# need to make a CMAKE file in the test/ folder
option(BUILD_TESTING "Build the testing tree" ON)
if(BUILD_TESTING AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing()
    add_subdirectory(test)
endif()

add_executable(client
        src/client/Client.cpp
        src/client/Client.h
        src/client/main.cpp
        src/client/User.h
        src/shared/sockio.cpp
        src/shared/sockio.h)

add_executable(server
        src/server/constsGlobsShared.h
        src/server/main.cpp
        src/server/Notifier.cpp
        src/server/Notifier.h
        src/server/Server.cpp
        src/server/Server.h
        src/server/Servlet.cpp
        src/server/Servlet.h
        src/server/User.h
        src/shared/sockio.cpp
        src/shared/sockio.h)

target_link_libraries(server Threads::Threads)

endif()
