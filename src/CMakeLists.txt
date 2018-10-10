find_library(LIBPQXX NAMES pqxx libpqxx)
find_library(LIBPQ NAMES pq libpq)

add_subdirectory(shared)

#include_directories("${PROJECT_SOURCE_DIR}/src/shared")

add_subdirectory(client)

add_subdirectory(server)

