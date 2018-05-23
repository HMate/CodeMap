cmake_minimum_required(VERSION 2.8.12)

project(CodeParser)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)

# *sigh* TODO: figure out how can find_package work without forcing client to create env vars
# Use absolute pathes for now, even though its ugly
# tried: compile llvm PACKAGE project, but it fails because
# nsis wants to create an exe thats too big
#   Internal compiler error #12345: error mmapping file (2085913006, 33554432) is out of range.
#   Note: you may have one or two (large) stale temporary file(s) left in your temporary directory (Generally this only happens on Windows 9x).
# set(Clang_DIR D:/prog/llvm_vs2017_x86_ver_60/tools/clang/cmake/modules/CMakeFiles)
# find_package(Clang)

SET(LLVM_BINARY_DIR_PATH "D:/prog/llvm_vs2017_x86_64_ver_60" CACHE PATH "Path to the root directory of llvm installation. This should contain the /libs and /includes directories.")
SET(LLVM_GEN_INCLUDE_PATH "${LLVM_BINARY_DIR_PATH}/include" CACHE PATH "/include dir for llvm headers")
SET(CLANG_GEN_INCLUDE_PATH "${LLVM_BINARY_DIR_PATH}/tools/clang/include" CACHE PATH "/include dir for generated clang headers")
SET(LLVM_AND_CLANG_DEBUG_LIBS_PATH "${LLVM_BINARY_DIR_PATH}/Debug/lib" CACHE PATH "Path to llvm and clang libraries")

SET(LLVM_SRC_ROOT_PATH "D:/prog/llvm" CACHE PATH "Path to llvm src root directory")
SET(LLVM_INCLUDE_PATH "${LLVM_SRC_ROOT_PATH}/include" CACHE PATH "Path to dir which contains the llvm header files")
SET(CLANG_INCLUDE_PATH "${LLVM_SRC_ROOT_PATH}/tools/clang/include" CACHE PATH "Path to dir which contains the clang header files")

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${LLVM_BINARY_DIR_PATH})
find_package(llvm REQUIRED)
find_package(clang REQUIRED)

add_library(${PROJECT_NAME} STATIC
    codeparser.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${LLVM_INCLUDE_PATH}
    PRIVATE ${LLVM_GEN_INCLUDE_PATH}
    PRIVATE ${CLANG_INCLUDE_PATH}
    PRIVATE ${CLANG_GEN_INCLUDE_PATH})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core

    LLVMCoverage
    LLVMProfileData

    clangTooling
    clangParse
    clangAnalysis
    clangRewriteFrontend

    version)

# LINK_DIRECTORIES(LLVM_AND_CLANG_DEBUG_LIBS_PATH)
