cmake_minimum_required(VERSION 2.8.12)

project(CodeMapGUI)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED Widgets Concurrent)

add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    settingsview.cpp
    settings.cpp
    terminalgui.cpp
    viewmanager.cpp
    
	diagramview.cpp
	includediagramview.cpp
	includetreediagrambuilder.cpp
    
	fileview.cpp
    fileedit.cpp
    textfolder.cpp
    linenumberarea.cpp
	editorfoldingarea.cpp

    filesystem.cpp
    appstate.cpp
    imagehandler.cpp
    stringlisteditordialog.cpp)

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