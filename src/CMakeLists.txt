add_subdirectory(shared)

#target_include_directories(sockio PUBLIC "${PROJECT_SOURCE_DIR}/src/shared")
include_directories("${PROJECT_SOURCE_DIR}/src/shared")
add_subdirectory(client)

add_subdirectory(server)

