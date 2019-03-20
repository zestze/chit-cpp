add_library(sockio
        sockio.cpp)

#-------------------------------
# let's any target that links against this library have access to these PUBLIC headers
#-------------------------------
target_include_directories(sockio
        PUBLIC
        "${PROJECT_SOURCE_DIR}/src/shared")
