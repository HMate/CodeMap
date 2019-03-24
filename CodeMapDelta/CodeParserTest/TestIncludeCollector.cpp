#include "catch2/catch.hpp"

#include "IncludeTreeBuilder.h"


// Test if the file is not a source code
TEST_CASE("Include tree builder", "[includes]")
{
    cm::IncludeTree tree;
    cm::IncludeTreeBuilder builder(tree);

    builder.setRoot("test", "testy");
    REQUIRE(builder.isRootSelected());
    REQUIRE(builder.getRoot().name() == "test");
    builder.addNode("include1", "full/name/of/include1");
    REQUIRE(builder.getRoot().includes().size() == 1);
    REQUIRE(builder.getRoot().includes()[0].name() == "include1");
    REQUIRE(builder.getRoot().includes()[0].path() == "full/name/of/include1");
    
    REQUIRE_FALSE(builder.selectNode("bad/name"));
    REQUIRE(builder.selectNode("full/name/of/include1"));
    REQUIRE(builder.currentNode().name() == "include1");
    REQUIRE(builder.currentNode().path() == "full/name/of/include1");

    builder.addNode("include_inner1", "full/name/of/include1/include_inner1");
    REQUIRE(builder.getRoot().includes().size() == 1);
    REQUIRE(builder.currentNode().includes().size() == 1);

    builder.addNode("include_inner2", "full/name/of/include1/include_inner2");
    REQUIRE(builder.getRoot().includes().size() == 1);
    REQUIRE(builder.currentNode().includes().size() == 2);


    REQUIRE(builder.selectNode("full/name/of/include1/include_inner2"));
    REQUIRE(builder.currentNode().name() == "include_inner2");
    REQUIRE(builder.currentNode().path() == "full/name/of/include1/include_inner2");

    builder.selectParent();
    REQUIRE(builder.currentNode().name() == "include1");
    REQUIRE(builder.currentNode().path() == "full/name/of/include1");

    builder.selectParent();
    REQUIRE(builder.isRootSelected());
    REQUIRE(builder.currentNode().name() == builder.getRoot().name());
    REQUIRE(builder.currentNode().path() == builder.getRoot().path());

    builder.selectParent();
    REQUIRE(builder.isRootSelected());
    REQUIRE(builder.currentNode().name() == builder.getRoot().name());
    REQUIRE(builder.currentNode().path() == builder.getRoot().path());
}

TEST_CASE("Include tree contains unique nodes", "[includes]")
{
    cm::IncludeTree tree;
    cm::IncludeTreeBuilder builder(tree);

    builder.setRoot("test", "testy");
    builder.addNode("include1", "root/include1");
    REQUIRE(builder.selectNode("root/include1"));
    builder.addNode("include_inner1", "root/include1/include_inner1");
    builder.addNode("include_inner2", "root/include1/include_inner2");

    REQUIRE(builder.selectNode("root/include1/include_inner2"));
    builder.addNode("include_inner3", "root/include1/include_inner3");
    builder.addNode("include_inner4", "root/include1/include_inner4");
    builder.addNode("include_inner5", "root/include1/include_inner5");

    builder.selectParent();
    builder.selectParent();
    builder.addNode("include_inner2", "root/include1/include_inner2");

    REQUIRE(tree.root().includes().size() == 2);
    REQUIRE(tree.root().includes()[1].path() == "root/include1/include_inner2");
    REQUIRE(tree.root().includes()[1].includes().size() == 3);
    REQUIRE(tree.root().includes()[1].includes()[0].path() == "root/include1/include_inner3");
    REQUIRE(tree.root().includes()[1].includes()[1].path() == "root/include1/include_inner4");
    REQUIRE(tree.root().includes()[1].includes()[2].path() == "root/include1/include_inner5");
}