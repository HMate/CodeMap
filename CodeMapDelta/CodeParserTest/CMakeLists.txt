cmake_minimum_required(VERSION 2.8.12)

project(CodeParserTest)

INCLUDE_DIRECTORIES(../CodeParser)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED Test)

add_executable(${PROJECT_NAME}
    TestParser.cpp)

target_link_libraries(${PROJECT_NAME} 
    Qt5::Test
    CodeParser)
    
# copy testPatterns after every cmake run next to test exe
file(COPY ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/TestPatterns
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    
if ( WIN32 )
    # Copy dlls for tests
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${QT_WINDEPLOY} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()