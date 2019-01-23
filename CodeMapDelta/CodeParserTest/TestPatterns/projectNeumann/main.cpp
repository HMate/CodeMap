#include "computer.h"
#include "monitor.h"

int main(int argc, char** argv)
{
    Gpu g;
    Monitor m;
    m.addGpu(g);
    Computer c(m);
    c.addRam(3);
    m.Show();
    return 0;
}