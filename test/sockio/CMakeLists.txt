add_executable(sockioTest sockioTest.cpp)

#target_link_libraries(sockioTest gtest gtest_main)
#target_link_libraries(sockioTest libgtest libgmock)
target_link_libraries(sockioTest ${GTEST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(sockioTest sockio)