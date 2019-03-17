add_library(chitter STATIC
        utils.cpp
        userFuncs.cpp
        serverFuncs.cpp
        channelFuncs.cpp)

#-------------------------------
# let's any target that links against this library have access to these PUBLIC headers
#-------------------------------
target_include_directories(chitter
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src/shared)

target_compile_features(chitter PUBLIC cxx_std_17)

file(COPY ${CONFIG_FILES}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(chitter ${CONAN_LIBS})
