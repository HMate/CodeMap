cmake_minimum_required(VERSION 3.9.6)

project(CodeMapLib)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED Widgets Concurrent)

add_library(${PROJECT_NAME} STATIC
    MainWindow.cpp
    SettingsView.cpp
    # Settings.cpp
    TerminalGui.cpp
    ViewManager.cpp
    
    Diagram/DiagramItems.cpp
    Diagram/DiagramView.cpp
    Diagram/IncludeTreeDiagram.cpp
    Diagram/IncludeDiagramView.cpp
    Diagram/IncludeTreeDiagramAligners.cpp
    Diagram/IncludeTreeDiagramBuilder.cpp
    Diagram/BoxPositioner.cpp
    Diagram/DiagramSerializer.cpp
    
    FileView.cpp
    FileEdit.cpp
    TextFolder.cpp
    LinenumberArea.cpp
    EditorFoldingArea.cpp

    FileSystem.cpp
    AppState.cpp
    ImageHandler.cpp
    StringListEditorDialog.cpp)

target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:CM_DEBUG>")

set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD 17
            )

target_include_directories(${PROJECT_NAME} 
    PRIVATE CodeParser)
    
# Make headers includeable by users of this library
target_include_directories(${PROJECT_NAME} INTERFACE ./)

target_link_libraries(${PROJECT_NAME} 
    Qt5::Widgets
    Qt5::Concurrent
    
    CodeParser)

file(COPY ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/gui_icons
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
