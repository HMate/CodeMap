cmake_minimum_required(VERSION 2.8.12)

project(CodeMapGUI)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Widgets)

# TODO: How to include headers from CodeParser
#target_include_directories(${PROJECT_NAME} ../CodeParser)

add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    terminalgui.cpp
    viewmanager.cpp
    fileview.cpp
    fileedit.cpp
    filesystem.cpp
    appstate.cpp)

target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

target_link_libraries(${PROJECT_NAME} CodeParser)
