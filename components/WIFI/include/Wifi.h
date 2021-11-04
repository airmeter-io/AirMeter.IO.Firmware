#pragma once
#include "Common.h"
#include "HttpServer.h"
#include<vector>
#include <string>
#include "WifiManager.h"


class WifiUIImplementation {
public:
   virtual void ProcessEvents() = 0;
   virtual void DisplayAPAuthInfo(const std::string& pSSID, const std::string& pPassword);

};

class Wifi {
  std::string _softApName;
  std::string _apPassword;

  bool _isProvisioning = false;

  WifiManager _wifiManager;
  WifiUIImplementation& _wifiUI;

public:
    Wifi(const std::string pSoftApName, const std::string pApPassword, WifiUIImplementation& pWifiUI);
    ~Wifi();
    
    bool IsProvisioned();
    void StartProvisioning();
    WifiManager& GetManager();
    wifi_ap_record_t *FindNetwork(const std::string& pSsid);
    bool TestStationCredentialsWPA(const wifi_ap_record_t * pAp, const std::string& pPassword, uint32_t pTimeout = 1000);
    bool SetStationCredentialsWPA(const wifi_ap_record_t * pAp, const std::string& pPassword);
    void Start();

};