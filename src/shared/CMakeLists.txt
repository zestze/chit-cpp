add_library(
        sockio
        sockio.h
        sockio.cpp
    )

target_include_directories(sockio PUBLIC  "${PROJECT_SOURCE_DIR}/src/shared")

#add_library(
#        chitter
#        chitter.cpp
#        chitter.h
#)

add_executable(
        chitter
        chitter.cpp
        chitter.h
)

target_link_libraries(chitter ${LIBPQXX} ${LIBPQ})
