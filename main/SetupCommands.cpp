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


GetAvailableWifiNetworksCommand::GetAvailableWifiNetworksCommand(GeneralSettings& pSettings, Wifi& pWifi) : _settings(pSettings), _wifi(pWifi) {

}
void GetAvailableWifiNetworksCommand::Process(Json& pJson,Json& pResult) {
    pResult.CreateStringProperty("Status","true");
    std::vector<Json*> networks;
    _wifi.GetManager().Scan();
    auto aps = _wifi.GetManager().GetAvailableAps();
    auto apCount = _wifi.GetManager().GetAvailableApCount();
    for(auto i = 0; i < apCount; i++)
    {
        auto network = new Json();
        network->CreateStringProperty("WifiSSID", (char*)(aps[i].ssid));
        char apMacAddr[20];
        snprintf(apMacAddr, sizeof(apMacAddr), "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",aps[i].bssid[0],aps[i].bssid[1],aps[i].bssid[2],aps[i].bssid[3],aps[i].bssid[4],aps[i].bssid[5]);
        network->CreateStringProperty("WifiApMacAddr",      apMacAddr);
        network->CreateNumberProperty("WifiChannel",        aps[i].primary);
        network->CreateNumberProperty("WifiSignalStrength", aps[i].rssi);
        network->CreateStringProperty("WifiAuthMode",           WifiManager::GetAuthModeText(aps[i].authmode));        
        networks.push_back(network);
    }
    pResult.CreateArrayProperty("Networks", networks);
}

std::string GetAvailableWifiNetworksCommand::GetName() {
    return "GETNETWORKS";
}

GetCurrentWifiNetworkCommand::GetCurrentWifiNetworkCommand(GeneralSettings& pSettings, Wifi& pWifi) : _settings(pSettings), _wifi(pWifi) {

}

void GetCurrentWifiNetworkCommand::Process(Json& pJson,Json& pResult) {
    pResult.CreateStringProperty("Status","true");
    pResult.CreateStringProperty("WifiSSID",_wifi.GetManager().GetSSID());
    pResult.CreateNumberProperty("WifiChannel",_wifi.GetManager().GetChannel());
    pResult.CreateStringProperty("WifiAuthMode", WifiManager::GetAuthModeText(_wifi.GetManager().GetAuthMode()));
    pResult.CreateStringProperty("WifiIP4Address",_wifi.GetManager().GetIPAddress());
    pResult.CreateStringProperty("WifiIP4Netmask",_wifi.GetManager().GetNetmask());    
    pResult.CreateStringProperty("WifiIP4Gateway",_wifi.GetManager().GetGateway());        
}

std::string GetCurrentWifiNetworkCommand::GetName() {
    return "GETNETWORKINFO";
}


SelectWifiNetworkCommand::SelectWifiNetworkCommand(GeneralSettings& pSettings, Wifi& pWifi) : _settings(pSettings), _wifi(pWifi) {
    
}

void SelectWifiNetworkCommand::TestConnectToNetwork() {
    _testingConnect = true;  
    _testSucceeded= _wifi.TestStationCredentialsWPA(_network, _password);
    _testingConnect = false;
}

void SelectWifiNetworkCommand::SetConnectToNetwork() {
    _applyingConnect = true;    
     _wifi.SetStationCredentialsWPA(_network, _password);   
    _applyingConnect = false;
}

void SelectWifiNetworkCommand::Process(Json& pJson,Json& pResult) {
    if(pJson.HasProperty("Mode")) {
        auto mode = pJson.GetStringProperty("Mode");
        if(mode=="Test" || mode == "Apply") {      
            if(!pJson.HasProperty("Ssid") || !pJson.HasProperty("Password") || !pJson.HasProperty("Id") ) {
                pResult.CreateBoolProperty("Status", false);
                return;
            }      
            auto lastId = pJson.GetIntProperty("Id");
            if(lastId == _lastId) return;
            _lastId = lastId;

            auto ssid = pJson.GetStringProperty("Ssid");
            _password = pJson.GetStringProperty("Password");
            _network = _wifi.FindNetwork(ssid);

            if(_network == nullptr) {
                pResult.CreateStringProperty("ConnectStatus", "NetworkNotFound");
            } else if(mode=="Test") {
                TestConnectToNetwork();
                pResult.CreateStringProperty("ConnectStatus",  "Testing");
            } else {
                SetConnectToNetwork();
                pResult.CreateStringProperty("ConnectStatus",  "Applying");
                        
            }
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

