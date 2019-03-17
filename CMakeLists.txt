cmake_minimum_required(VERSION 3.10)
project(chit_cpp VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include(.conan/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS) # by default this macro overrides where binary is built to

# ------------------
# resource files such
# as db config should
# be passed to each target that
# requires them
# ------------------
file(GLOB CONFIG_FILES
    ${PROJECT_SOURCE_DIR}/res/*.xml)

add_subdirectory(src)

add_subdirectory(test)
