cmake_minimum_required(VERSION 2.8.12)

project(CodeParserTest)

INCLUDE_DIRECTORIES(../CodeParser)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)


find_package(Qt5Test REQUIRED)

add_executable(${PROJECT_NAME}
    TestParser.cpp)

add_custom_target(CodeParserTestHeaders SOURCES
    TestParser.h)

target_link_libraries(${PROJECT_NAME} Qt5::Test)
target_link_libraries(${PROJECT_NAME} CodeParser)
