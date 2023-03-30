cmake_minimum_required(VERSION 3.0.0)
project(sunflower VERSION 0.1.0)

include(CTest)
enable_testing()
include_directories(.)

add_subdirectory(tests)

add_library(sunflower
    header.hpp
    linkedlist.hpp linkedlist.cpp
    lexer/token.hpp lexer/token.cpp
    lexer/lexer.hpp lexer/lexer.cpp
    parser/ast.hpp parser/ast.cpp
    parser/sffunc.hpp parser/sffunc.cpp
    ip/module.hpp ip/module.cpp
    ip/name_lookup.hpp ip/name_lookup.cpp
    ip/ip.hpp ip/ip.cpp
    sunflower.hpp sunflower.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
