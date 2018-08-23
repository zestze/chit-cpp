

#find_library(LIBPQXX libpqxx)
#find_library(LIBPQ   libpq)

find_library(LIBPQXX pqxx)
find_library(LIBPQ pq)


add_subdirectory(shared)

include_directories("${PROJECT_SOURCE_DIR}/src/shared")
add_subdirectory(client)

add_subdirectory(server)

