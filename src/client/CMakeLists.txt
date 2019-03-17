add_executable(client
        Client.cpp
        Client.h
        main.cpp)

target_link_libraries(client sockio)
target_link_libraries(client chitter)
target_link_libraries(client Threads::Threads)
