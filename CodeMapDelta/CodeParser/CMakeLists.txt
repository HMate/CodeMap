cmake_minimum_required(VERSION 2.8.12)

project(CodeParser)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)

add_library(${PROJECT_NAME} STATIC
    parser.cpp)

add_custom_target(CodeParserHeaders SOURCES
    parser.h)

target_link_libraries(${PROJECT_NAME} Qt5::Core)
