cmake_minimum_required(VERSION 3.10)
project(chit_cpp)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

include_directories(src/client)
include_directories(src/shared)
include_directories(src/server)

add_executable(client
        src/client/Client.cpp
        src/client/Client.h
        src/client/Main.cpp
        src/client/User.h
        src/shared/Sockio.cpp
        src/shared/Sockio.h)

add_executable(server
        src/server/ConstsGlobsShared.h
        src/server/Main.cpp
        src/server/Notifier.cpp
        src/server/Notifier.h
        src/server/Server.cpp
        src/server/Server.h
        src/server/Servlet.cpp
        src/server/Servlet.h
        src/server/User.h
        src/shared/Sockio.cpp
        src/shared/Sockio.h)

target_link_libraries(server Threads::Threads)

