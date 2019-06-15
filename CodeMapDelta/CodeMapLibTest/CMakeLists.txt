cmake_minimum_required(VERSION 2.8.12)

project(CodeMapLibTest LANGUAGES CXX)

include_directories(../CodeMapLib)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Catch2 CONFIG REQUIRED)
find_package(Qt5 CONFIG REQUIRED Core)

add_executable(${PROJECT_NAME}
	Test.cpp
    TestDiagram.cpp)


target_link_libraries(${PROJECT_NAME} 
	Catch2::Catch2
	Qt5::Core
    CodeMapLib)
    
if ( WIN32 )
    # Copy dlls for tests
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${QT_WINDEPLOY} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()