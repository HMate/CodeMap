#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include <cstdio>

int main(int argc, char* argv[]) {

    int result = Catch::Session().run(argc, argv);

    std::cin.ignore();
    return result;
}
