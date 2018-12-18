#pragma once
#include "ram.h"
#include "monitor.h"

class Computer
{
    Ram m_ram;
public:
    Computer(const Monitor);

    void addRam(int amount);
    Ram getRam();
};
