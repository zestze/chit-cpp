add_library(configUtils
        configUtils.cpp)

#@TODO: in this and in sockio, should public headers be shared/sockio, not shared?
#-------------------------------
# let's any target that links against this library have access to these PUBLIC headers
#-------------------------------
target_include_directories(configUtils
        PUBLIC
        "${PROJECT_SOURCE_DIR}/src/shared")

file(COPY ${CONFIG_FILES}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
