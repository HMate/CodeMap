#include "catch2/catch.hpp"

#include "TestParser.h"
#include "CodeParser.h"


TEST_CASE("Preprocess from string", "[preprocessor][cpp]")
{
    QString code =
            "#define TEST 2\n"
            "int add2(int input){\n"
            "   return input+TEST\n"
            "}";
    QString expected =
            "\nint add2(int input){\n"
            "   return input+2\n"
            "}\n";
    QString result = cm::CodeParser().getPreprocessedCode(code);
    REQUIRE(result == expected);
}

/* Test if preprocessor substitution is working*/
TEST_CASE("Preprocess from file", "[preprocessor][cpp]")
{
    QString codePath = getTestPatternPath("testPreprocessorWithFile", "test.cpp");
    QString expected = readFileContent(getTestPatternPath("testPreprocessorWithFile", "result.cpp"));

    auto result = cm::CodeParser().getPreprocessedCodeFromPath(codePath);
    REQUIRE(result.code.content == expected);
}

void check_include(cm::ParserResult result, int index, const QString& filename, long firstLine, long lastLine)
{
    CHECK(result.code.includes[index].filename == filename);
    CHECK(result.code.includes[index].firstLine == firstLine);
    CHECK(result.code.includes[index].lastLine == lastLine);
}

/* Test if preprocessor include substitution is working,
 * when the include is inside the same folder as the cpp*/
TEST_CASE("Preprocess if there are includes", "[preprocessor][cpp][includes]")
{
    QString codePath = getTestPatternPath("testPreprocessorInclude", "test.cpp");
    QString expected = readFileContent(getTestPatternPath("testPreprocessorInclude", "result.cpp"));

    auto result = cm::CodeParser().getPreprocessedCodeFromPath(codePath);
    REQUIRE(result.code.content == expected);
    REQUIRE(result.code.includes.size() == 1);
    check_include(result, 0, "lib.h", 1, 5);
}

/* Test if preprocessor include substitution is working,
 * when the include is outside of the cpp folder*/
TEST_CASE("Preprocess if include is outside own directory", "[preprocessor][cpp][includes]")
{
    QString codePath = getTestPatternPath("testPreprocessorIncludeOuterDir", "test.cpp");
    QString expected = readFileContent(getTestPatternPath("testPreprocessorIncludeOuterDir", "result.cpp"));

    auto result = cm::CodeParser().getPreprocessedCodeFromPath(codePath, {getTestPatternPath("testPreprocessorIncludeOuterDir", "externalDependencies")});
    REQUIRE(result.code.content == expected);
    REQUIRE(result.code.includes.size() == 1);
    check_include(result, 0, "lib.h", 1, 5);
}

TEST_CASE("Preprocess a mini project", "[preprocessor][cpp][includes]")
{
    QString codePath = getTestPatternPath("projectNeumann", "main.cpp");
    
    auto result = cm::CodeParser().getPreprocessedCodeFromPath(codePath);
    
    REQUIRE(result.code.includes.size() == 4);
    check_include(result, 0, "ram.h", 3, 10);
    check_include(result, 1, "gpu.h", 14, 20);
    check_include(result, 2, "monitor.h", 11, 29);
    check_include(result, 3, "computer.h", 1, 39);
}

// Test if we can include stdlib code
TEST_CASE("Preprocess if include is std lib", "[preprocessor][cpp]")
{
    QString codePath = getTestPatternPath("testPreprocessorIncludeStdio", "test.cpp");
    QString expected = readFileContent(getTestPatternPath("testPreprocessorIncludeStdio", "result.cpp"));

    auto result = cm::CodeParser().getPreprocessedCodeFromPath(codePath);
    // Its complicated to actually compare the whole file content, because every define is expanded in stdio.h
    // just see if the method result contains the needed code + some stdio functions
    REQUIRE(result.code.content.contains(expected));
    REQUIRE(result.code.content.contains("__cdecl fopen")); // see if result contains something from <stdio.h>
}

// Test if the file is not a source code
TEST_CASE("Preprocess non source code file", "[preprocessor][cpp]")
{
    QString codePath = getTestPatternPath("testPreprocessorNonSourceCode", "test.cpp");
    QString expected = readFileContent(getTestPatternPath("testPreprocessorNonSourceCode", "test.cpp"));

    auto result = cm::CodeParser().getPreprocessedCodeFromPath(codePath);
    REQUIRE(result.code.content.contains(expected));
}

// Test parsing the inlcude hierarchy of a file
TEST_CASE("Parse include hierarchy", "[includes][cpp]")
{
    QString codePath = getTestPatternPath("projectNeumann", "main.cpp");

    auto result = cm::CodeParser().getIncludeTree(codePath);
    REQUIRE(result->root().name() == "main.cpp");
    REQUIRE(result->root().isFullInclude());
    REQUIRE(result->root().includes().size() == 2);
    REQUIRE(result->root().includes()[0].name() == "computer.h");
    REQUIRE(result->root().includes()[0].includes().size() == 2);
    REQUIRE(result->root().includes()[0].includes()[0].name() == "ram.h");
    REQUIRE(result->root().includes()[0].includes()[0].isFullInclude());
    REQUIRE(result->root().includes()[0].includes()[0].includes().size() == 0);
    REQUIRE(result->root().includes()[0].includes()[1].name() == "monitor.h");
    REQUIRE(result->root().includes()[0].includes()[1].isFullInclude());
    REQUIRE(result->root().includes()[0].includes()[1].includes().size() == 1);
    REQUIRE(result->root().includes()[0].includes()[1].includes()[0].name() == "gpu.h");
    REQUIRE(result->root().includes()[0].includes()[1].includes()[0].includes().size() == 0);
    REQUIRE(result->root().includes()[1].name() == "monitor.h");
    REQUIRE(result->root().includes()[1].isFullInclude() == false);
    REQUIRE(result->root().includes()[1].includes().size() == 0); // not a full include, we only show what is actually included
}

// Test include hierarchy parsing, when includes are recursive, and dont have a header guard
TEST_CASE("Parse recursive includes without header guard", "[includes][cpp]")
{
    QString codePath = getTestPatternPath("testIncludeRecursive", "main.cpp");

    auto result = cm::CodeParser().getIncludeTree(codePath);
    REQUIRE(result->root().name() == "main.cpp");
    REQUIRE(result->root().includes().size() == 1);
    REQUIRE(result->root().includes()[0].name() == "recursive.h");
    REQUIRE(result->root().includes()[0].isFullInclude());
    // because of the unguarded recursion, the parser will fail to parse the sources, and the compilations fails.
    // So it doesnt really matter what is the include hierarchy in this case. I just leave this test here so we now
    // that this case was acknowledged.
}

// Test include hierarchy parsing, when includes are recursive, and dont have a header guard
TEST_CASE("Parse includes when an include is not in path", "[includes][cpp]")
{
    QString codePath = getTestPatternPath("testPreprocessorIncludeOuterDir", "test.cpp");

    auto result = cm::CodeParser().getIncludeTree(codePath);
    REQUIRE(result->root().name() == "test.cpp");
    REQUIRE(result->root().includes().size() == 1);
    REQUIRE(result->root().includes()[0].name() == "lib.h");
    REQUIRE(result->root().includes()[0].isFullInclude() == false);
}

// Test if the file is not a source code
TEST_CASE("Parse AST", "[ast][cpp]")
{
    QString codePath = getTestPatternPath("projectNeumann", "main.cpp");

    cm::CodeParser().parseAST(codePath);
}

