#include "SetupCommands.h"
#include "GeneralSettings.h"
#include "Json.h"
#include <string>
#include "System.h"

SaveSettingsCommand::SaveSettingsCommand(GeneralSettings& pSettings) : _settings(pSettings) {
    
}
void SaveSettingsCommand::Process(Json& pJson,Json& pResult)  {
    pResult.CreateStringProperty("Status","true");
    
    if(pJson.HasProperty("DeviceName")) 
        _settings.SetDeviceName(pJson.GetStringProperty("DeviceName"));
    
    if(pJson.HasProperty("EnableMqtt")) 
        _settings.SetEnableMqtt(pJson.GetBoolProperty("EnableMqtt"));
    
    if(pJson.HasProperty("EnableDhcpNtp")) 
        _settings.SetEnableDhcpNtp(pJson.GetBoolProperty("EnableDhcpNtp"));

    if(pJson.HasProperty("EnablePowerSave")) 
        _settings.SetEnablePowerSave(pJson.GetBoolProperty("EnablePowerSave"));
    
    if(pJson.HasProperty("PrimaryNtpServer")) 
        _settings.SetPrimaryNtpServer(pJson.GetStringProperty("PrimaryNtpServer"));
    
    if(pJson.HasProperty("SecondaryNtpServer")) 
        _settings.SetSecondaryNtpServer(pJson.GetStringProperty("SecondaryNtpServer"));

    if(pJson.HasProperty("MqttServerAddress")) 
        _settings.SetMqttServerAddress(pJson.GetStringProperty("MqttServerAddress"));
    
    if(pJson.HasProperty("MqttTopic")) 
        _settings.SetMqttTopic(pJson.GetStringProperty("MqttTopic"));

    if(pJson.HasProperty("MqttPublishDelay")) 
        _settings.SetMqttPublishDelay(pJson.GetIntProperty("MqttPublishDelay"));                
    
    if(pJson.HasProperty("Co2SensorType"))
    {
        auto sensor = pJson.GetIntProperty("Co2SensorType");
        if(sensor < (int)CO2SensorType::MaxType && sensor >= 0) {
            _settings.SetCO2SensorType((CO2SensorType)sensor);
        }
    }

    if(pJson.HasProperty("SensorUpdateInterval"))
    {
        auto interval = pJson.GetIntProperty("SensorUpdateInterval");
        if(interval < (int)3600 && interval > 0) {
            _settings.SetSensorUpdateInterval(interval);
        }

    }
    _settings.Save();
}
std::string SaveSettingsCommand::GetName()  {
    return "SAVESETTINGS";
}



LoadSettingsCommand::LoadSettingsCommand(GeneralSettings& pSettings) : _settings(pSettings) {
    
}
void LoadSettingsCommand::Process(Json& pJson,Json& pResult)  {
    pResult.CreateStringProperty("Status","true");
    pResult.CreateStringProperty("DeviceName",_settings.GetDeviceName());
    pResult.CreateBoolProperty("EnableMqtt",_settings.GetEnableMqtt());
    pResult.CreateBoolProperty("EnableDhcpNtp",_settings.GetEnableDhcpNtp());
    pResult.CreateBoolProperty("EnablePowerSave",_settings.GetEnablePowerSave());
    pResult.CreateStringProperty("PrimaryNtpServer",_settings.GetPrimaryNtpServer());
    pResult.CreateStringProperty("SecondaryNtpServer",_settings.GetSecondaryNtpServer());
    pResult.CreateStringProperty("MqttServerAddress",_settings.GetMqttServerAddress());
    pResult.CreateStringProperty("MqttTopic",_settings.GetMqttTopic());
    pResult.CreateNumberProperty("MqttPublishDelay",_settings.GetMqttPublishDelay());
    pResult.CreateNumberProperty("Co2SensorType", (int)_settings.GetCO2SensorType());
    pResult.CreateNumberProperty("SensorUpdateInterval", (int)_settings.GetSensorUpdateInterval());

}
std::string LoadSettingsCommand::GetName() {
    return "LOADSETTINGS";
}


GetSystemInfoCommand::GetSystemInfoCommand(GeneralSettings& pSettings) : _settings(pSettings) {
    
}
void GetSystemInfoCommand::Process(Json& pJson,Json& pResult)  {
    pResult.CreateStringProperty("Status","true");
    pResult.CreateStringProperty("MCUName",System::GetMCUName());
    pResult.CreateNumberProperty("CoreCount",(int)System::GetCoreCount());
    pResult.CreateNumberProperty("FreeHeap",(int)System::GetFreeHeap());
    pResult.CreateNumberProperty("LeastFreeHeap",(int)System::GetLeastHeapFreeSinceBoot());
}

std::string GetSystemInfoCommand::GetName() {
    return "SYSTEMINFO";
}


GetAvailableWifiNetworksCommand::GetAvailableWifiNetworksCommand(GeneralSettings& pSettings, WifiTask& pWifi) : _settings(pSettings), _wifi(pWifi) {

}
void GetAvailableWifiNetworksCommand::Process(Json& pJson,Json& pResult) {
    pResult.CreateStringProperty("Status","true");
    std::vector<Json*> networks;
    WifiAvailableNetworks availableNetworks;
    _wifi.ScanForAvailableNetworks(availableNetworks);
    auto configured = _wifi.GetNetworks();
    for(auto availableNetwork : availableNetworks)
    {
        if(configured.contains(availableNetwork->ssid)) continue;
        auto network = new Json();
        network->CreateStringProperty("ssid", availableNetwork->ssid.c_str());
        char apMacAddr[20];
        snprintf(apMacAddr, sizeof(apMacAddr), "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
                availableNetwork->bssid[0],
                availableNetwork->bssid[1],
                availableNetwork->bssid[2],
                availableNetwork->bssid[3],
                availableNetwork->bssid[4],
                availableNetwork->bssid[5]);
        network->CreateStringProperty("apMacAddr",      apMacAddr);
        network->CreateNumberProperty("channel",       availableNetwork->channel);
        network->CreateNumberProperty("signalStrength", availableNetwork->rssi);
        network->CreateStringProperty("authMode",       availableNetwork->authmode);        
        networks.push_back(network);
    }
    pResult.CreateArrayProperty("Networks", networks);
}

std::string GetAvailableWifiNetworksCommand::GetName() {
    return "GETNETWORKS";
}

GetCurrentWifiNetworkCommand::GetCurrentWifiNetworkCommand(GeneralSettings& pSettings) : _settings(pSettings) {

}

void GetCurrentWifiNetworkCommand::Process(Json& pJson,Json& pResult) {
    pResult.CreateStringProperty("Status","true");
    for(auto groupPair : ValueController::GetCurrent().GetGroups()) {
        Json* groupJson = nullptr;
        for(auto keyPair : groupPair.second->SourcesByName) {
            auto source = keyPair.second->DefaultSource;
            if(source->GetFlags() & NETWORK_INFO ) {
                if(groupJson == nullptr) 
                    groupJson = pResult.CreateObjectProperty(groupPair.first);
                source->SerialiseToJsonProperty(*groupJson);
            }            
        }   
        if(groupJson!=nullptr)
            delete groupJson; 
    }   
}

std::string GetCurrentWifiNetworkCommand::GetName() {
    return "GETNETWORKINFO";
}


SelectWifiNetworkCommand::SelectWifiNetworkCommand(GeneralSettings& pSettings, WifiTask& pWifi) : _settings(pSettings), _wifi(pWifi) {
    
}

void SelectWifiNetworkCommand::TestConnectToNetwork() {
    printf("testing network\n");
    _testingConnect = true;  
    _testSucceeded= _wifi.TestConfiguration(_ssid, _auth, _password);
    printf("Test success? == %s\n", _testSucceeded ? "yes" : "no");
    _testingConnect = false;
}

void SelectWifiNetworkCommand::SetConnectToNetwork() {
    printf("Applying network\n");
    _applyingConnect = true;    
     _wifi.AddConfiguration(_ssid, _auth, _password, _makeDefault);   
    _applyingConnect = false;
}

void SelectWifiNetworkCommand::Process(Json& pJson,Json& pResult) {
    if(pJson.HasProperty("Mode")) {
        auto mode = pJson.GetStringProperty("Mode");
        if(mode=="Test" || mode == "Apply") {   
            if(_testingConnect || _applyingConnect) {
                printf("already testing\n");
                return;   
            }
            if(!pJson.HasProperty("Ssid") || !pJson.HasProperty("Password")  || !pJson.HasProperty("Auth") || !pJson.HasProperty("Id") || 
               (mode=="Apply" && !pJson.HasProperty("MakeDefault") ) ) {
                pResult.CreateBoolProperty("Status", false);
                return;
            }      
            auto lastId = pJson.GetIntProperty("Id");
            if(lastId == _lastId) return;
            _lastId = lastId;

            _ssid = pJson.GetStringProperty("Ssid");
            _password = pJson.GetStringProperty("Password");
            _auth = pJson.GetStringProperty("Auth");

            if(mode=="Test") {
                TestConnectToNetwork();
                pResult.CreateStringProperty("ConnectStatus",  "Testing");
            } else {
                _makeDefault = pJson.GetBoolProperty("MakeDefault");
                SetConnectToNetwork();
                pResult.CreateStringProperty("ConnectStatus",  "Applying");
                        
            }
        } else if (mode == "List") {
            std::vector<Json*> networks;
            auto connectionInfo = _wifi.GetConnectionInfo();
            for(auto configuredNetwork : _wifi.GetNetworks()) {
                auto network = new Json();
                network->CreateStringProperty("ssid", configuredNetwork.second->ssid);
                network->CreateStringProperty("authMode", configuredNetwork.second->authMode);
                network->CreateNumberProperty("priority", (int)configuredNetwork.second->priority); 
                printf("Configured SSID = %s, current SSID=%s\n", configuredNetwork.second->ssid.c_str(), connectionInfo->ssid.c_str());
                if(configuredNetwork.second->ssid == connectionInfo->ssid) {
                    auto connectionJson = network->CreateObjectProperty("connection");
                    connectionJson->CreateNumberProperty("channel", (int)connectionInfo->channel); 
                    connectionJson->CreateStringProperty("ipv4Address", connectionInfo->ipv4Address);
                    connectionJson->CreateStringProperty("ipv4Gateway", connectionInfo->ipv4Gateway);
                    connectionJson->CreateStringProperty("ipv4Netmask", connectionInfo->ipv4Netmask);
                    delete connectionJson;
                }              
                networks.push_back(network);
            }
            delete connectionInfo;
            pResult.CreateArrayProperty("Networks", networks);                        
        } else if (mode == "Remove") {
            if(!pJson.HasProperty("Ssid"))
            {
                pResult.CreateBoolProperty("Status", false);
                return;
            }
            auto ssid = pJson.GetStringProperty("Ssid");
            auto connectionInfo = _wifi.GetConnectionInfo();
            if(ssid == connectionInfo->ssid ||! _wifi.RemoveConfiguration(ssid)) {
                delete connectionInfo;
                pResult.CreateBoolProperty("Status", false);
                return;
            }
            delete connectionInfo;
        } else {
            pResult.CreateBoolProperty("Testing", _testingConnect);
            pResult.CreateBoolProperty("TestSuccess", _testSucceeded);
            pResult.CreateBoolProperty("Applying", _applyingConnect);
        }                

        pResult.CreateBoolProperty("Status", true);
    } else {
        pResult.CreateBoolProperty("Status", false);
    }
}

std::string SelectWifiNetworkCommand::GetName() {
    return "SELECTNETWORK";   
}

DataManagementCommand::DataManagementCommand(DataManager& pManager) : _manager(pManager) {

}

void DataManagementCommand::Process(Json& pJson,Json& pResult) {
    _manager.ClearAllData();
    pResult.CreateBoolProperty("Status", true);
}

std::string DataManagementCommand::GetName() {
    return "MANAGEDATA";
}

