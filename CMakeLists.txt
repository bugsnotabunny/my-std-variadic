cmake_minimum_required(VERSION 3.5.0)
project(my-std-variadic VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC_DIR src)
set(INC_DIR inc)
set(EX_DIR exp)
set(TEST_DIR tests)
set(EXT_DIR ext)
set(EXSRC_DIR ${EX_DIR}/${SRC_DIR})

add_compile_options(-g -Wall -Wextra -Wold-style-cast -Werror -Wno-missing-field-initializers)

file(GLOB_RECURSE LIB_SRCS ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE EX_SRCS ${EXSRC_DIR}/*.cpp)
file(GLOB_RECURSE TEST_SRCS ${TEST_DIR}/*.cpp)

file(GLOB EX_MAINS ${EX_DIR}/*.cpp)

find_package(Catch2 3 REQUIRED)
add_executable(utests ${TEST_SRCS})
target_link_libraries(utests PRIVATE Catch2::Catch2WithMain)
target_include_directories(utests PRIVATE ${INC_DIR})


# add_library(watcher ${LIB_SRCS})
# target_link_libraries(watcher systemd)

# add_executable(utests ${TEST_SRCS} ${EX_SRCS} ${EXT_DIR}/catch2/catch_amalgamated.cpp)
# target_include_directories(utests PRIVATE ${INC_DIR} ${EXSRC_DIR} ${EXT_DIR}/catch2)
# target_link_libraries(utests watcher)

foreach(EX_MAIN IN LISTS EX_MAINS)
    get_filename_component(EXECUTABLE ${EX_MAIN} NAME_WE)
    add_executable(${EXECUTABLE} ${EX_MAIN} ${EX_SRCS})
    # target_link_libraries(${EXECUTABLE} watcher)
    target_include_directories(${EXECUTABLE} PRIVATE ${INC_DIR} ${EXSRC_DIR})
endforeach()