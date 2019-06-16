#include "catch2/catch.hpp"

#include "Utils.hpp"

#include "IncludeTreeBuilder.h"
#include "Diagram/IncludeDiagramView.h"
#include "Diagram/IncludeTreeDiagramBuilder.h"

TEST_CASE("Create a diagram from code", "[]")
{
    IncludeDiagramView * view = new IncludeDiagramView();

    cm::IncludeTree tree;
    cm::IncludeTreeBuilder builder(tree);
    builder.setRoot("test", "testy");
    builder.addNode("include1", "root/include1");
    builder.addNode("include2", "root/include2");

    buildIncludeTreeDiagram(*view, tree);
    auto& diagram = view->getDiagram();
    
    REQUIRE(2 == diagram.size());
    REQUIRE(1 == diagram[0].size());
    REQUIRE("testy" == diagram[0].front().m_box->getFullName());
    REQUIRE(2 == diagram[1].size());
    REQUIRE("root/include1" == diagram[1][0].m_box->getFullName());
    REQUIRE("root/include2" == diagram[1][1].m_box->getFullName());
}
