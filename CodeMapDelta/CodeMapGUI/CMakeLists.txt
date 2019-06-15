cmake_minimum_required(VERSION 3.9.6)

project(CodeMapGUI)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED Widgets)

add_executable(${PROJECT_NAME}
    Main.cpp)

target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:CM_DEBUG>")

set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD 17
            )

target_include_directories(${PROJECT_NAME} 
    PRIVATE CodeMapLib)

target_link_libraries(${PROJECT_NAME} 
    Qt5::Widgets
    CodeMapLib)

file(COPY ${CMAKE_SOURCE_DIR}/CodeMapLib/gui_icons
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
    
if ( WIN32 )
    # Copy dlls to exe
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${QT_WINDEPLOY} $<TARGET_FILE_DIR:${PROJECT_NAME}>) 
endif()
