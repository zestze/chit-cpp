add_executable(client
        Client.cpp
        Client.h
        main.cpp
    )

target_link_libraries(client sockio)
