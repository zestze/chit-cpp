add_executable(client
        Client.cpp
        Client.h
        main.cpp
        User.h
    )

target_link_libraries(client sockio)
