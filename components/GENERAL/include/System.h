#pragma once

#include "Common.h"
#include <string>

class System {
public:
    static void Restart();
    static char *GetMCUName();
    static int GetCoreCount();
    static uint32_t GetFreeHeap();
    static uint32_t GetLeastHeapFreeSinceBoot();
    
};