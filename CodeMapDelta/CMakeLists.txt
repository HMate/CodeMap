cmake_minimum_required(VERSION 2.8.11)
project(CodeMapDelta)

message("\nStart cmake generation of " ${PROJECT_NAME} "\n")
message("Cmake module: " ${CMAKE_MODULE_PATH} )
message("Cmake root path: " ${CMAKE_ROOT} )
message("Cmake system prefix: " ${CMAKE_SYSTEM_PREFIX_PATH} )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions(-DQT_DEPRECATED_WARNINGS)

add_subdirectory(CodeParser)
add_subdirectory(CodeMapGUI)


add_subdirectory(CodeParserTest)
