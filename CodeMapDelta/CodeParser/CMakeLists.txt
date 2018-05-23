cmake_minimum_required(VERSION 2.8.12)

project(CodeParser)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)

SET(LLVM_BINARY_DIR_PATH "D:/prog/llvm_vs2017_x86_64_ver_60" CACHE PATH "Path to the root directory of llvm installation. This should contain the /libs and /includes directories.")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${LLVM_BINARY_DIR_PATH})

find_package(llvm REQUIRED)
find_package(clang REQUIRED)

add_library(${PROJECT_NAME} STATIC
    codeparser.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${LLVM_INCLUDE_DIR}
    PRIVATE ${CLANG_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core

    LLVMCoverage
    LLVMProfileData

    clangTooling
    clangParse
    clangAnalysis
    clangRewriteFrontend

    version)
