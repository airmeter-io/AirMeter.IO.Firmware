#pragma once
#include "SettingsBase.h"
#include<string>
#include<map>

class WifiConfiguredNetwork {
public:
    std::string ssid;
    std::string password;
    std::string authMode;
    uint32_t priority;
} ;

typedef WifiConfiguredNetwork* PWifiConfiguredNetwork;

class WifiSettings : public SettingsBase {
private: 
    std::map<std::string,PWifiConfiguredNetwork> _networks;
    time_t _waitOnDisconnectedTime = 120;
protected:
    const char* GetFilePath() override;
    void LoadSettingsFromJson(Json& pJson) override;
    void SaveSettingsToJson(Json& pJson) override;
public:
    WifiSettings();  

    inline std::map<std::string, PWifiConfiguredNetwork>& GetNetworks() { return _networks; };
    inline time_t GetWaitOnDisconnectTime() { return _waitOnDisconnectedTime; };
    inline void SetWaitOnDisconnectTime(time_t pWaitTime) { _waitOnDisconnectedTime = pWaitTime; };
};