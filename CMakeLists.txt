cmake_minimum_required(VERSION 3.14)
project(dfa)

set(CMAKE_CXX_STANDARD 20)
find_package(fmt REQUIRED)

add_executable(dfa dfa.c++)
target_link_libraries(dfa fmt::fmt)
