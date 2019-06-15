cmake_minimum_required(VERSION 2.8.11)
project(CodeMapDelta)

message("\nStart cmake generation of " ${PROJECT_NAME} "\n")
message("Cmake module: " ${CMAKE_MODULE_PATH} )
message("Cmake root path: " ${CMAKE_ROOT} )
message("Cmake system prefix: " ${CMAKE_SYSTEM_PREFIX_PATH} )


set(QT_LIB_ROOT_PATH "C:/Qt/5.10.1/msvc2017_64" CACHE PATH "Path to qt5 libs and cmake root")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_LIB_ROOT_PATH})

# windeployqt.exe is for copying dlls to build directory
set(QT_WINDEPLOY ${QT_LIB_ROOT_PATH}\\bin\\windeployqt.exe)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if ( MSVC )
    # turn on all warnings
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W3")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /wd4100 /wd4141 /wd4146 /wd4267 /wd4291")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
endif()

add_definitions(-DQT_DEPRECATED_WARNINGS)

add_subdirectory(CodeParser)
add_subdirectory(CodeMapLib)
add_subdirectory(CodeMapGUI)

#Tests
add_subdirectory(CodeParserTest)
add_subdirectory(CodeMapLibTest)
