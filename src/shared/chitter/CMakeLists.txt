add_library(chitter STATIC
        chitter.h convenienceFuncs.h utils.cpp utils.h userFuncs.cpp userFuncs.h serverFuncs.cpp serverFuncs.h channelFuncs.cpp channelFuncs.h)

target_compile_features(chitter PUBLIC cxx_std_17)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(chitter ${LIBPQXX} ${LIBPQ} stdc++fs)
