add_executable(chitterTest chitterTest.cpp)

target_link_libraries(chitterTest ${CONAN_LIBS})
target_link_libraries(chitterTest chitter)

file(COPY ${CONFIG_FILES}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
