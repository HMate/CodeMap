#include "catch2/catch.hpp"

#include "IncludeTreeBuilder.h"


// Test if the file is not a source code
TEST_CASE("Include tree builder", "[includes]")
{
    cm::IncludeTree tree;
    cm::IncludeTreeBuilder builder(tree);

    builder.setRoot("test", "testy");
    REQUIRE(builder.isRootSelected());
    REQUIRE(builder.currentNode().id() == 0);
    builder.addNode("include1", "full/name/of/include1");
    REQUIRE(builder.getRoot().includes().size() == 1);
    REQUIRE(builder.getRoot().includes()[0].id() == 1);
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
    REQUIRE(builder.currentNode().id() == 3);
    REQUIRE(builder.currentNode().name() == "include_inner2");
    REQUIRE(builder.currentNode().path() == "full/name/of/include1/include_inner2");

    builder.selectPreviousNode();
    REQUIRE(builder.currentNode().name() == "include1");
    REQUIRE(builder.currentNode().path() == "full/name/of/include1");

    builder.selectPreviousNode();
    REQUIRE(builder.isRootSelected());
    REQUIRE(builder.currentNode().name() == builder.getRoot().name());
    REQUIRE(builder.currentNode().path() == builder.getRoot().path());

    builder.selectPreviousNode();
    REQUIRE(builder.isRootSelected());
    REQUIRE(builder.currentNode().name() == builder.getRoot().name());
    REQUIRE(builder.currentNode().path() == builder.getRoot().path());

    REQUIRE(tree.root().name() == "test");
}

TEST_CASE("Include tree contains duplicate nodes", "[includes]")
{
    cm::IncludeTree tree;
    cm::IncludeTreeBuilder builder(tree);

    builder.setRoot("test", "testy");
    builder.addNode("include1", "root/include1");
    builder.addNode("include_inner2", "root/include1/include_inner2").setFullInclude(false);

    REQUIRE(builder.selectNode("root/include1"));
    builder.addNode("include_inner1", "root/include1/include_inner1");
    builder.addNode("include_inner2", "root/include1/include_inner2");

    REQUIRE(builder.selectNode("root/include1/include_inner2"));
    builder.currentNode().setFullInclude(true);
    builder.addNode("include_inner3", "root/include1/include_inner3");
    builder.addNode("include_inner4", "root/include1/include_inner4");
    builder.addNode("include_inner5", "root/include1/include_inner5");

    REQUIRE(tree.root().id() == 0);
    REQUIRE(tree.root().includes().size() == 2);
    REQUIRE(tree.root().includes()[0].includes()[1].path() == "root/include1/include_inner2");
    REQUIRE(tree.root().includes()[0].includes()[1].isFullInclude());
    REQUIRE(tree.root().includes()[0].includes()[1].includes().size() == 3);
    REQUIRE(tree.root().includes()[0].includes()[1].includes()[0].path() == "root/include1/include_inner3");
    REQUIRE(tree.root().includes()[0].includes()[1].includes()[1].path() == "root/include1/include_inner4");
    REQUIRE(tree.root().includes()[0].includes()[1].includes()[2].path() == "root/include1/include_inner5");

    REQUIRE(tree.root().includes()[1].path() == "root/include1/include_inner2");
    REQUIRE(!tree.root().includes()[1].isFullInclude());
    REQUIRE(tree.root().includes()[1].includes().size() == 0);
}

TEST_CASE("Include node equality", "[includes]")
{
    cm::IncludeNode node1(1, "test", "fullpath", true);
    cm::IncludeNode node2(2, "other", "fullpath2", true);
    cm::IncludeNode node3(3, "other", "fullpath", false);
    cm::IncludeNode node4(3, "other", "fullpath", true);
    REQUIRE_FALSE(node1 == node2);
    REQUIRE(node1 == node3);
    REQUIRE(node1 == node4);
}