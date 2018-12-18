#include "catch2/catch.hpp"

#include "TestParser.h"
#include "codeparser.h"


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

/* Test if preprocessor include substitution is working,
 * when the include is inside the same folder as the cpp*/
TEST_CASE("Preprocess if there are includes", "[preprocessor][cpp]")
{
    QString codePath = getTestPatternPath("testPreprocessorInclude", "test.cpp");
    QString expected = readFileContent(getTestPatternPath("testPreprocessorInclude", "result.cpp"));

    auto result = cm::CodeParser().getPreprocessedCodeFromPath(codePath);
    REQUIRE(result.code.content == expected);
    REQUIRE(result.includes.size() == 1);
    CHECK(result.includes[0].filename == "lib.h");
    CHECK(result.includes[0].firstLine == 1);
    CHECK(result.includes[0].lastLine == 4);
}

/* Test if preprocessor include substitution is working,
 * when the include is outside of the cpp folder*/
TEST_CASE("Preprocess if include is outside own directory", "[preprocessor][cpp]")
{
    QString codePath = getTestPatternPath("testPreprocessorIncludeOuterDir", "test.cpp");
    QString expected = readFileContent(getTestPatternPath("testPreprocessorIncludeOuterDir", "result.cpp"));

    auto result = cm::CodeParser().getPreprocessedCodeFromPath(codePath, {getTestPatternPath("testPreprocessorIncludeOuterDir", "externalDependencies")});
    REQUIRE(result.code.content == expected);
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

