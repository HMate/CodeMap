#include "catch2/catch.hpp"

#include <QString>

TEST_CASE("Create a diagram from code", "[]")
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
    REQUIRE(code == expected);
}
