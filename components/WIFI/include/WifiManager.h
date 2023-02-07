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


class WifiManager : private WifiManagerInternal {
    bool _isApStarted = false;
    bool _isStaConnected = false;
    wifi_config_t _wifiConfig;
    wifi_init_config_t _initConfig = WIFI_INIT_CONFIG_DEFAULT();
    const std::string _softApName;
    esp_netif_t *_staIf;
    std::string _apPassword;
    std::string _ipAddress;
    std::string _netmask;
    std::string _gateway;
    std::string _ssid;
    bool _wifiIsStarted = false;
    int _channel;
    wifi_auth_mode_t _authMode;
    wifi_ap_record_t* _availableAps = nullptr;
    uint32_t _availableApCount = 0;
    
    std::vector<wifi_config_t> _staConfigStack;
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

   
        
public:
    WifiManager();
    ~WifiManager();

    void PushStaConfig();
    void PopStaConfig();
    void SetMemoryMode();
    void SetFlashMode();
    bool HasStationConfiguration();
    bool EnableAP(const std::string& pApName, const std::string& pPassword, uint8_t pMaxConnections = 5);
    bool EnableStationOnly();
    bool DisableWifi();
    bool IsStationConnected();
    bool ConnectStation();
    bool DisconnectStation();


    void Scan();
    bool ConfigureStationAP(const wifi_ap_record_t * pAp, const std::string& pPassword);
    wifi_ap_record_t* GetAvailableAps();
    uint32_t GetAvailableApCount();
    const std::string GetApPassword();
    const std::string GetApName();
    std::string& GetIPAddress();
    std::string& GetNetmask();
    std::string& GetGateway();
    bool GetIsProvisioning();
    
    std::string& GetSSID();
    int GetChannel();
    wifi_auth_mode_t GetAuthMode();

    static std::string GetAuthModeText(wifi_auth_mode_t pAuthMode);
};