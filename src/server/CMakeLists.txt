add_executable(server
        globals.h
        main.cpp
        Notifier.cpp
        Notifier.h
        Server.cpp
        Server.h
        Servlet.cpp
        Servlet.h)

target_link_libraries(server Threads::Threads sockio)

target_link_libraries(server chitter)
