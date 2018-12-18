#ifndef MONITOR_H
#define MONITOR_H
#include "gpu.h"

class Monitor
{
    Gpu m_gpu;
public:
    addGpu(const Gpu& gpu);
    Gpu& getGpu();
    void Show();
};
#endif //MONITOR_H