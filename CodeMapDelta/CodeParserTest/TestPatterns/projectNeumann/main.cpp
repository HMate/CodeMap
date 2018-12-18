#include "computer.h"
#include "monitor.h"

int main(char argc, char** argv)
{
    Gpu g;
    Monitor m(g);
    Computer c(m);
    c.addRam(3);
    m.Show();
    return 0;
}