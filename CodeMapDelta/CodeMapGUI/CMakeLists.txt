cmake_minimum_required(VERSION 2.8.12)

project(CodeMapGUI)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED Widgets Concurrent)

add_executable(${PROJECT_NAME}
    Main.cpp
    MainWindow.cpp
    SettingsView.cpp
    Settings.cpp
    TerminalGui.cpp
    ViewManager.cpp
    
    DiagramView.cpp
    IncludeDiagramView.cpp
    IncludeTreeDiagramBuilder.cpp
    
    FileView.cpp
    FileEdit.cpp
    TextFolder.cpp
    LinenumberArea.cpp
    EditorFoldingArea.cpp

    FileSystem.cpp
    AppState.cpp
    ImageHandler.cpp
    StringListEditorDialog.cpp)

target_include_directories(${PROJECT_NAME} 
    PRIVATE CodeParser)

target_link_libraries(${PROJECT_NAME} 
    Qt5::Widgets
    Qt5::Concurrent
    
    CodeParser)

file(COPY ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/gui_icons
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)

if ( WIN32 )
    # Copy dlls to exe
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${QT_WINDEPLOY} $<TARGET_FILE_DIR:${PROJECT_NAME}>) 
endif()