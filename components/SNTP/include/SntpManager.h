#pragma once
#include "Common.h"
#include<vector>
#include<string>

class SntpManager {  
public:
    SntpManager(std::vector<std::string*> pNtpServers, bool pUseDHCP);
    void Init();
    ~SntpManager();
};
