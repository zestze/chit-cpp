add_executable(sockioTest sockioTest.cpp)

target_link_libraries(sockioTest ${CONAN_LIBS})
target_link_libraries(sockioTest sockio)