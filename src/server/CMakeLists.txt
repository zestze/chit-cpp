add_executable(server
        globals.h
        main.cpp
        Notifier.cpp
        Notifier.h
        Server.cpp
        Server.h
        Servlet.cpp
        Servlet.h)

target_link_libraries(server Threads::Threads sockio chitter)

file(COPY ${CONFIG_FILES}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
