#include "catch2/catch.hpp"

#include <iostream>
#include "Utils.hpp"

#include "IncludeTreeBuilder.h"
#include "Diagram/IncludeDiagramView.h"
#include "Diagram/IncludeTreeDiagramBuilder.h"
#include "Diagram/DiagramSerializer.h"

TEST_CASE("Create a diagram from code", "[diagram]")
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

TEST_CASE("Serialize diagram", "[diagram][serialization]")
{
    IncludeDiagramView * view = new IncludeDiagramView();

    cm::IncludeTree tree;
    cm::IncludeTreeBuilder builder(tree);
    builder.setRoot("test", "testy");
    builder.addNode("include1", "root/include1");
    builder.addNode("include2", "root/include2");

    buildIncludeTreeDiagram(*view, tree);
    QString result = DiagramSerializer::serialize(view->getDiagram());

    QString expected = R"Text({
    "type": "IncludeDiagram",
    "version": "1.0",
    "diagram": {
        "edges": { "0": [1,2] },
        "nodes": [
            { 
                "id": 0,
                "name": "test",
                "path": "testy",
                "fullInclude": "true"
            },
            { 
                "id": 1,
                "name": "include1",
                "path": "root/include1",
                "fullInclude": "true"
            },
            { 
                "id": 2,
                "name": "include2",
                "path": "root/include2",
                "fullInclude": "true"
            }
        ]
    }
})Text";

    REQUIRE_JSON(expected, result);
}

TEST_CASE("Serialize with duplicate nodes in diagram", "[diagram][serialization]")
{
    IncludeDiagramView* view = new IncludeDiagramView();

    cm::IncludeTree tree;
    cm::IncludeTreeBuilder builder(tree);
    builder.setRoot("test", "testy");
    builder.addNode("include1", "root/include1");
    builder.addNode("include2", "root/include2");
    builder.selectNode("root/include2");
    builder.addNode("include1", "root/include1");

    buildIncludeTreeDiagram(*view, tree);
    QString result = DiagramSerializer::serialize(view->getDiagram());

    QString expected = QStringLiteral(R"Text({
    "type": "IncludeDiagram",
    "version": "1.0",
    "diagram": {
        "edges": {
            "0": [1,2],
            "2": [3]
        },
        "nodes": [
            { 
                "id": 0,
                "name": "test",
                "path": "testy",
                "fullInclude": "true"
            },
            { 
                "id": 1,
                "name": "include1",
                "path": "root/include1",
                "fullInclude": "true"
            },
            { 
                "id": 2,
                "name": "include2",
                "path": "root/include2",
                "fullInclude": "true"
            },
            { 
                "id": 3,
                "name": "include1",
                "path": "root/include1",
                "fullInclude": "false"
            }
        ]
    }
})Text");

    REQUIRE_JSON(expected, result);
}
