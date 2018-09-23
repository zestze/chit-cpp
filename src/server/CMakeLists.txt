add_executable(server
        constsGlobsShared.h
        main.cpp
        Notifier.cpp
        Notifier.h
        Server.cpp
        Server.h
        Servlet.cpp
        Servlet.h
        DbPool.cpp DbPool.h)

target_link_libraries(server Threads::Threads sockio)

target_link_libraries(server chitter)

add_executable(dbPool
        DbPool.cpp
        DbPool.h)
target_link_libraries(dbPool chitter)
