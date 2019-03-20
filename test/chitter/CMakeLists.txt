add_executable(chitterTest chitterTest.cpp)

target_link_libraries(chitterTest ${CONAN_LIBS})
#@TODO: is there a way to link against chitter without having to link against configUtils?
target_link_libraries(chitterTest chitter configUtils)

file(COPY ${CONFIG_FILES}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
