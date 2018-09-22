cmake_minimum_required(VERSION 2.8.12)

project(CMakeParser)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

SET(LIBCMAKE_BINARY_DIR_PATH "D:/prog/cmake_3_12_2" CACHE PATH "Path to the root directory of cmake libraries. This should contain the /libs and /includes directories.")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${LIBCMAKE_BINARY_DIR_PATH})

set(LIBCMAKE_INCLUDES_PATH "D:/prog/cmake/Source" CACHE PATH "Path to sources/haders of cmake")


# find_package(CMakeLib REQUIRED)

add_library(${PROJECT_NAME} STATIC
    cmakeparser.cpp)

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

target_include_directories(${PROJECT_NAME} 
	PRIVATE ${LIBCMAKE_INCLUDES_PATH})

# Make headers includeable by users of this library
target_include_directories(${PROJECT_NAME} INTERFACE ./)

target_link_libraries(${PROJECT_NAME}
    version)
