cmake_minimum_required(VERSION 3.14)
project(dfa)

set(CMAKE_CXX_STANDARD 20)

add_executable(
    dfa
    main.c++
    dfa.c++
    input.c++
    dpda.c++
)
