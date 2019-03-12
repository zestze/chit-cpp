add_executable(chitterTest chitterTest.cpp)

target_link_libraries(chitterTest ${CONAN_LIBS})
target_link_libraries(chitterTest chitter)

file(COPY ../../src/shared/chitter/config
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
