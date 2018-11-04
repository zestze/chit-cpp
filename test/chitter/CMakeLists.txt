add_executable(chitterTest chitterTest.cpp)

#target_link_libraries(chitterTest gtest gtest_main)
#target_link_libraries(chitterTest libgtest libgmock)
target_link_libraries(chitterTest ${GTEST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(chitterTest chitter)

file(COPY ../../src/shared/chitter/config
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
