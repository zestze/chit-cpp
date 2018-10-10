add_library(
        sockio
        sockio.h
        sockio.cpp
    )

target_include_directories(sockio PUBLIC  "${PROJECT_SOURCE_DIR}/src/shared")

add_library(chitter STATIC
        chitter.cpp
        chitter.h
)

target_compile_features(chitter PUBLIC cxx_std_17)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(chitter ${LIBPQXX} ${LIBPQ} stdc++fs)
