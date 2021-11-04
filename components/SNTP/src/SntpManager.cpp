#include "SntpManager.h"

#define TAG "SNTP"

SntpManager::SntpManager(std::vector<std::string*> pNtpServers, bool pUseDHCP) {
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);

    if(pUseDHCP) {
         sntp_servermode_dhcp(1);
    } else {  
        int index = 0;
        for(auto ntpServer : pNtpServers)  {
            sntp_setservername(index, ntpServer->c_str());
            printf("%d: %s\n",index, ntpServer->c_str());
            index++;
        }
    }
}

void SntpManager::Init() {
    sntp_init();
}

SntpManager::~SntpManager() {

}