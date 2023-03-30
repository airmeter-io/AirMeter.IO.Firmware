#pragma once

#include "ValueController.h"
#include "WifiSettings.h"
#include "GeneralSettings.h"
#include "WifiManager.h"
#include <string>
#include "freertos/semphr.h"

class WifiManagerPrivate {
public:
    virtual void ApTaskMain() = 0;
    virtual void StationConnectLoop() = 0;
};

class WifiConnectionInfo {
public:
    std::string ssid;
    std::string authMode;
    uint8_t channel;
    std::string ipv4Address;
    std::string ipv4Gateway;
    std::string ipv4Netmask;
};

enum WifiTaskConnectPhase {
    Starting,
    StartConnectSequence,
    Searching,
    SearchResults,
    TryConnect,
    Connecting,
    WaitingForIP,
    Connected,
    Failed,
    Disconnected,
    TestSuccess,
    TestFailure
};

class WifiTask:  public ValuesSource, private WifiManagerPrivate, private WifiManagerCallBacks {
private:
    std::string _apSSID = "";
    std::string _apAuth = "";
    std::string _apIPv4Gateway ="192.168.4.1";
    std::string _apIPv4StartIP ="192.168.4.4";
    std::string _apIPv4Mask = "255.255.255.0";    
    std::string _apPassword = "";
    std::string _staSSID = "";
    std::string _staAuth = "";
    std::string _staIPv4Addr = "0.0.0.0";
    std::string _staIPv4Mask = "255.255.255.255"; 
    std::string _staIPv4Gateway = "0.0.0.0";

    Value _valIsStaConnected { .b = false };  
    Value _valStaSSID { .s = &_staSSID };    
    Value _valStaAuth { .s = &_staAuth };    
    Value _valStaRssi { .i = 0 };    
    Value _valStaIPv4Addr { .s = &_staIPv4Addr };  
    Value _valStaIPv4Mask { .s = &_staIPv4Mask };  
    Value _valStaIPv4Gateway { .s = &_staIPv4Gateway };  

    Value _valChannel { .i = 0 };    
    
    Value _valApIPv4Gateway { .s = &_apIPv4Gateway };  
    Value _valApIPv4StartIP { .s = &_apIPv4StartIP };  
    Value _valApIPv4Mask { .s = &_apIPv4Mask };  
    Value _valIsApStaConnected { .b = false };        
    Value _valIsApActive { .b = false };  
    Value _valApSSID { .s = &_apSSID };  
    Value _valApAuth { .s = &_apAuth };  
    Value _valApPassword { .s = &_apPassword };           
    static WifiSettings* _wifiSettings;
    WifiManager* _manager;
    WifiTaskConnectPhase _phase = WifiTaskConnectPhase::Starting;
    WifiConfiguredNetwork* _testConfiguration = nullptr;
    bool _gotIP;
    time_t _phaseStart;
    SemaphoreHandle_t _stationSemaphore = nullptr;
    SemaphoreHandle_t _searchSemaphore = nullptr;
    SemaphoreHandle_t _uiMutex = nullptr;
    void ApTaskMain() override;
    void StationConnectLoop() override;

    void CreateAPTaskIfNotRunning();
    void SetPhase(WifiTaskConnectPhase pPhase);
    static bool CompareAvailableNetworks(WifiAvailableNetwork* pNetwork1, WifiAvailableNetwork* pNetwork2);
    WifiConfiguredNetwork* GetConfiguredNetwork(const std::string& pSSID, const std::string& pAuthMode);

    void OnWifiStarted() override;
    void OnStationConnected(std::string pSSID, uint8_t pChanel, uint8_t pBSSID[6], std::string pAuthMode) override;
    void OnStationDisconnected(WifiDisconnectReason pReason)  override;
    void OnStationGotIP(std::string pIp, std::string pNetmask, std::string pGateway) override;
    void OnStationLostIP() override;
    void OnScanComplete() override;
   
public:
     WifiTask(const std::string& pDeviceName, const std::string& pApPassword);
     ~WifiTask();
     const std::string& GetValuesSourceName() const override;   
     void Init();   
     bool HasConfiguredNetworks();

    void ScanForAvailableNetworks(WifiAvailableNetworks& pNetworks);
    bool TestConfiguration(const std::string& pSSID, const std::string& pAuthMode, const std::string& pPassword);
    bool AddConfiguration(const std::string& pSSID, const std::string& pAuthMode, const std::string& pPassword, bool pMakePriority);
    bool RemoveConfiguration(std::string pSSID);
    inline std::map<std::string, PWifiConfiguredNetwork>& GetNetworks() { return _wifiSettings->GetNetworks(); };
    WifiConnectionInfo* GetConnectionInfo();
};