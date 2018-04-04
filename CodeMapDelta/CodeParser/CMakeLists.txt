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

SET(LLVM_INCLUDE_PATH "D:/prog/llvm/include")
SET(LLVM_GEN_INCLUDE_PATH "D:/prog/llvm_vs2017_x86_64_ver_60/include")

SET(CLANG_INCLUDE_PATH "D:/prog/llvm/tools/clang/include")
SET(CLANG_GEN_INCLUDE_PATH "D:/prog/llvm_vs2017_x86_64_ver_60/tools/clang/include")

SET(LLVM_AND_CLANG_DEBUG_LIBS_PATH "D:/prog/llvm_vs2017_x86_64_ver_60/Debug/lib")

add_library(${PROJECT_NAME} STATIC
    codeparser.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${LLVM_INCLUDE_PATH}
    PRIVATE ${LLVM_GEN_INCLUDE_PATH}
    PRIVATE ${CLANG_INCLUDE_PATH}
    PRIVATE ${CLANG_GEN_INCLUDE_PATH})

MACRO(FIND_LLVM_LIBS _libname_)
    find_library(${_libname_}_LIBRARY ${_libname_} HINTS ${LLVM_AND_CLANG_DEBUG_LIBS_PATH})
    set(LLVM_LIBRARIES ${LLVM_LIBRARIES} ${${_libname_}_LIBRARY})
endmacro()

FIND_LLVM_LIBS(LLVMSupport)
FIND_LLVM_LIBS(LLVMOption)
FIND_LLVM_LIBS(LLVMBitReader)
FIND_LLVM_LIBS(LLVMCoverage)
FIND_LLVM_LIBS(LLVMProfileData)
FIND_LLVM_LIBS(LLVMMC)
FIND_LLVM_LIBS(LLVMMCParser)
FIND_LLVM_LIBS(LLVMCore)
FIND_LLVM_LIBS(LLVMBinaryFormat)

FIND_LLVM_LIBS(ClangAST)
FIND_LLVM_LIBS(ClangBasic)
FIND_LLVM_LIBS(ClangEdit)
FIND_LLVM_LIBS(clangFrontend)
FIND_LLVM_LIBS(clangSerialization)
FIND_LLVM_LIBS(clangDriver)
FIND_LLVM_LIBS(clangTooling)
FIND_LLVM_LIBS(clangParse)
FIND_LLVM_LIBS(clangSema)
FIND_LLVM_LIBS(clangAnalysis)
FIND_LLVM_LIBS(clangRewriteFrontend)
FIND_LLVM_LIBS(clangRewrite)
FIND_LLVM_LIBS(clangLex)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    ${LLVM_LIBRARIES}
    version)

# LINK_DIRECTORIES(LLVM_AND_CLANG_DEBUG_LIBS_PATH)
