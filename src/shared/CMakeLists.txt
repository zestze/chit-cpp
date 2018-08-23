add_library(
        sockio
        sockio.h
        sockio.cpp
    )

target_include_directories(sockio PUBLIC  "${PROJECT_SOURCE_DIR}/src/shared")
