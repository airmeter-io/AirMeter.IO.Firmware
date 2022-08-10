#pragma once
#include "Common.h"
#include "HttpServer.h"
#include<vector>
#include <string>
#include "WifiManager.h"


class WifiPrivate {
public:
   virtual void RunCaptiveDNS() = 0;
  

};

class Wifi : private WifiPrivate {
  std::string _softApName;
  std::string _apPassword;

  bool _isProvisioning = false;

  WifiManager _wifiManager;

  void RunCaptiveDNS() override;
public:
    Wifi(const std::string pSoftApName, const std::string pApPassword);
    ~Wifi();
    
    bool IsProvisioned();
    bool IsConnected();
    void StartProvisioning();
    WifiManager& GetManager();
    wifi_ap_record_t *FindNetwork(const std::string& pSsid);
    bool TestStationCredentialsWPA(const wifi_ap_record_t * pAp, const std::string& pPassword, uint32_t pTimeout = 1000);
    bool SetStationCredentialsWPA(const wifi_ap_record_t * pAp, const std::string& pPassword);
    void Start();

};