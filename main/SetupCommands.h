#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "Wifi.h"
#include "GeneralSettings.h"
#include "Json.h"
#include "DataManager.h"
#include <string>


class SaveSettingsCommand : public HttpJsonCommand  {
    GeneralSettings& _settings;
    public:
        SaveSettingsCommand(GeneralSettings& pSettings);
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};


class LoadSettingsCommand : public HttpJsonCommand  {
    GeneralSettings& _settings;
    public:
        LoadSettingsCommand(GeneralSettings& pSettings)  ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};

class GetSystemInfoCommand : public HttpJsonCommand  {
    GeneralSettings& _settings;
    public:
        GetSystemInfoCommand(GeneralSettings& pSettings)  ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};

class GetAvailableWifiNetworksCommand : public HttpJsonCommand  {
    GeneralSettings& _settings;
    Wifi& _wifi;
    public:
        GetAvailableWifiNetworksCommand(GeneralSettings& pSettings, Wifi& pWifi) ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};


class GetCurrentWifiNetworkCommand : public HttpJsonCommand  {
    GeneralSettings& _settings;
    Wifi& _wifi;
    public:
        GetCurrentWifiNetworkCommand(GeneralSettings& pSettings, Wifi& pWifi) ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};


class SelectWifiNetworkCommandInternal {
public:
    virtual void TestConnectToNetwork() = 0;
    virtual void SetConnectToNetwork() = 0;
};

class SelectWifiNetworkCommand : public HttpJsonCommand, private SelectWifiNetworkCommandInternal  {
    GeneralSettings& _settings;
    Wifi& _wifi;
    int _lastId = 0;
    bool _testingConnect = false;
    bool _applyingConnect = false;
    bool _testSucceeded = false;
    wifi_ap_record_t * _network = nullptr;
    std::string _password;
    void TestConnectToNetwork() override;
    void SetConnectToNetwork() override;
public:
    SelectWifiNetworkCommand(GeneralSettings& pSettings, Wifi& pWifi) ;
    void Process(Json& pJson,Json& pResult) override ;
    std::string GetName() override ;
};


class DataManagementCommand : public HttpJsonCommand  {
    DataManager& _manager;
public:
    DataManagementCommand(DataManager& pManager) ;
    void Process(Json& pJson,Json& pResult) override ;
    std::string GetName() override ;
};