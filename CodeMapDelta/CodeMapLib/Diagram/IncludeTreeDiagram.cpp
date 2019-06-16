#include "IncludeTreeDiagram.h"

BoxBuilder& BoxBuilder::operator=(const BoxBuilder & other)
{
    m_node = other.m_node;
    m_box = other.m_box;
    return *this;
}

bool BoxBuilder::operator==(const BoxBuilder& o)
{
    return m_box == o.m_box;
}