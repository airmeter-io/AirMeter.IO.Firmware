#pragma once
#include "Common.h"
#include<vector>
#include <string>
#include <functional>


extern "C" {
   // #include "esp_netif.h"
    #include "esp_wifi.h"
}

class WifiManagerInternal {
    public:
       virtual void ProcessWifiEvent(wifi_event_t pEvent, void *pEventData);
       virtual void ProcessIPEvent(ip_event_t pEvent, void *pEventData);
};


class WifiAvailableNetwork {
public:
    uint8_t bssid[6];
    std::string ssid;
    uint8_t channel;
    std::string authmode;
    int rssi;
} ;


enum WifiMode {
    Disabled,
    Station,
    StationAP
};

typedef struct {
    std::string ssid;
    std::string password;
    std::string startIP;
    std::string gatewayIP;
    std::string netmask;    
} ApDescription;

enum WifiDisconnectReason {
    AuthFailed,
    ConnectionFailed,
    LostConnection
};

class WifiAvailableNetworks : public std::vector<WifiAvailableNetwork*> {
private:
    bool _freeOnDispose  = true;
public:
    WifiAvailableNetworks(bool freeOnDispose = true);
    ~WifiAvailableNetworks();

    void clearAndFree();
};

class WifiManagerCallBacks {
public:
    virtual void OnWifiStarted() = 0;
    virtual void OnStationConnected(std::string pSSID, uint8_t pChanel, uint8_t pBSSID[6], std::string pAuthMode) = 0;
    virtual void OnStationDisconnected(WifiDisconnectReason pReason)  = 0;
    virtual void OnStationGotIP(std::string pIp, std::string pNetmask, std::string pGateway) = 0;
    virtual void OnStationLostIP() = 0;
    virtual void OnScanComplete() = 0;
};


class WifiManager : private WifiManagerInternal {
    WifiMode _mode = WifiMode::Disabled;

    bool _isApStarted = false;
    bool _isStaConnected = false;
    wifi_config_t _wifiConfig;
    wifi_init_config_t _initConfig = WIFI_INIT_CONFIG_DEFAULT();
    const std::string _softApName;
    esp_netif_t *_staIf;
    esp_netif_t *_apIf;
    std::string _netmask;
    std::string _gateway;
    bool _wifiIsStarted = false;
    WifiManagerCallBacks& _callbacks;
   
    void ProcessWifiEvent(wifi_event_t pEvent, void *pEventData) override;
    void ProcessIPEvent(ip_event_t pEvent, void *pEventData) override;
    void ProcessGotIPEvent(ip_event_got_ip_t* pEvent);
    void ProcessLostIPEvent();
    void ProcessGotIPV6Event(ip_event_got_ip6_t* pEvent);
    void ProcessGotStaAssignedIPEvent(ip_event_ap_staipassigned_t* pEvent);     
    void ProcessWifiReadyEvent();
    void ProcessScanDoneEvent(wifi_event_sta_scan_done_t* pEvent);
    void ProcessWifiStationStartEvent();
    void ProcessWifiStationStopEvent();
    void ProcessWifiStationConnectedEvent(wifi_event_sta_connected_t* pEvent);
    void ProcessWifiStationDisconnectedEvent();
    void ProcessWifiStationAuthmodeChangeEvent(wifi_event_sta_authmode_change_t *pEvent);
    void ProcessWifiStationSignalToLow();
    void ProcessAccessPointStartEvent();
    void ProcessAccessPointStopEvent();
    void ProcessWifiAccessPointStationConnectedEvent(wifi_event_ap_staconnected_t* pEvent);
    void ProcessWifiAccessPointStationDisconnectedEvent(wifi_event_ap_stadisconnected_t* pEvent);       
    void ProcessWifiAccessPointProbeReqRecievedEvent(wifi_event_ap_probe_req_rx_t* pEvent);    

   
    static std::string GetAuthModeText(wifi_auth_mode_t pAuthMode);    
    static wifi_auth_mode_t GetAuthModeFromText(std::string pAuthMode);
public:
    WifiManager(WifiManagerCallBacks& pCallbacks);
    ~WifiManager();
  
    bool HasStationConfiguration();
    bool EnableAP(const ApDescription& pAp, uint8_t pMaxConnections = 5);
    bool DisableAP();  
    bool EnableWifi();  
    bool DisableWifi();
    bool ConnectStation(const WifiAvailableNetwork* pNetwork, const std::string& pPassword);
    bool DisconnectStation();

    void Scan();
    void GetScanResults(WifiAvailableNetworks& pAvailableNetworks);


};