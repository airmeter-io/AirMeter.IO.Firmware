#include "GeneralSettings.h"
#include "Json.h"
#include <string>
#include <stdio.h>

extern "C" {
    #include "esp_mac.h"
}

GeneralSettings::GeneralSettings() {
    uint8_t macAddress[6];
    esp_efuse_mac_get_default(macAddress);
    _deviceName = "AIO";
    char hex[14];
    snprintf(hex, sizeof(hex), "%.4X",*(uint16_t*)(macAddress+4) );
    _deviceName+=hex;

    Load();
}

const char * GeneralSettings::GetFilePath() {
    return "/spiffs/device.json";
}

void GeneralSettings::LoadSettingsFromJson(Json& pJson) {
    if(pJson.HasProperty("DeviceNetName"))
        _deviceName = pJson.GetStringProperty("DeviceNetName");
    if(pJson.HasProperty("ApPassword"))
        _apPassword = pJson.GetStringProperty("ApPassword");        
    if(pJson.HasProperty("EnableMqtt"))
        _enableMqtt = pJson.GetBoolProperty("EnableMqtt");
    if(pJson.HasProperty("EnableDhcpNtp"))
        _enableDhcpNtp = pJson.GetBoolProperty("EnableDhcpNtp");
    if(pJson.HasProperty("MqttServerAddress"))
        _mqttServerAddress = pJson.GetStringProperty("MqttServerAddress");
    if(pJson.HasProperty("MqttTopic"))
        _mqttTopic = pJson.GetStringProperty("MqttTopic");    
    if(pJson.HasProperty("MqttPublishSecondDelay"))
        _mqttPublishSecondDelay = pJson.GetIntProperty("MqttPublishSecondDelay");           
    if(pJson.HasProperty("CO2SensorType")) 
        _sensorType = (CO2SensorType)pJson.GetIntProperty("CO2SensorType");
    if(pJson.HasProperty("SensorUpdateInterval")) 
        _sensorUpdateInterval = (CO2SensorType)pJson.GetIntProperty("SensorUpdateInterval");            
    if(pJson.HasProperty("EnablePowerSave")) {
        _enablePowerSave = pJson.GetBoolProperty("EnablePowerSave");
    }
}

void GeneralSettings::SaveSettingsToJson(Json& pJson) {    
    pJson.CreateStringProperty("DeviceNetName", _deviceName);
    pJson.CreateStringProperty("ApPassword", _apPassword);
    pJson.CreateBoolProperty("EnableMqtt", _enableMqtt);
    pJson.CreateBoolProperty("EnableDhcpNtp", _enableDhcpNtp);
    pJson.CreateStringProperty("MqttTopic", _mqttTopic);
    pJson.CreateNumberProperty("MqttPublishSecondDelay", _mqttPublishSecondDelay);
    pJson.CreateStringProperty("MqttServerAddress", _mqttServerAddress);
    pJson.CreateNumberProperty("CO2SensorType", (int)_sensorType);
    pJson.CreateNumberProperty("SensorUpdateInterval", _sensorUpdateInterval);
    pJson.CreateBoolProperty("EnablePowerSave", _enablePowerSave);
}

std::string& GeneralSettings::GetMqttServerAddress() {
  return _mqttServerAddress;
}

void GeneralSettings::SetMqttServerAddress(const std::string& pValue) {
   _mqttServerAddress = pValue;
}

bool GeneralSettings::GetEnableMqtt() { 
    return _enableMqtt;
}

void GeneralSettings::SetEnableMqtt(bool pValue) {
    _enableMqtt = pValue;
}

int GeneralSettings::GetMqttPublishDelay() {
    return _mqttPublishSecondDelay;
}

void GeneralSettings::SetMqttPublishDelay(int pDelay) {
    _mqttPublishSecondDelay = pDelay;
}

bool GeneralSettings::GetEnableDhcpNtp() {
    return _enableDhcpNtp;
}
void GeneralSettings::SetEnableDhcpNtp(bool pValue) {
    _enableDhcpNtp = pValue;
}

std::vector<std::string*> GeneralSettings::GetNtpServers() {
    std::vector<std::string*> result;
    if(_primaryNtpServer.size()>0)
        result.push_back(&_primaryNtpServer);
    if(_secondaryNtpServer.size()>0)
        result.push_back(&_secondaryNtpServer);
    return result;
}

void GeneralSettings::SetPrimaryNtpServer(const std::string &pServer) {
    _primaryNtpServer = pServer;
}

void GeneralSettings::SetSecondaryNtpServer(const std::string &pServer) {
    _secondaryNtpServer = pServer;
}

std::string GeneralSettings::GetPrimaryNtpServer() {
    return _primaryNtpServer;
}

std::string GeneralSettings::GetSecondaryNtpServer() {
    return _secondaryNtpServer;
}

std::string GeneralSettings::GetDeviceName() {
    return _deviceName;
}

void GeneralSettings::SetDeviceName(const std::string& pDeviceName) {
    _deviceName = pDeviceName;
}

std::string GeneralSettings::GetApPassword() {
    return _apPassword;
}

void GeneralSettings::SetApPassword(const std::string& pPassword) {
    _apPassword = pPassword;
}

std::string GeneralSettings::GetMqttTopic() {
    return _mqttTopic;
}

void GeneralSettings::SetMqttTopic(const std::string& pTopic) {
    _mqttTopic = pTopic;
}

CO2SensorType GeneralSettings::GetCO2SensorType() {
    return _sensorType;
}

void GeneralSettings::SetCO2SensorType(CO2SensorType pSensorType) {
    _sensorType = pSensorType;
}


int GeneralSettings::GetSensorUpdateInterval() {
    return _sensorUpdateInterval;
}

void GeneralSettings::SetSensorUpdateInterval(int pIntervalInSeconds) {
    _sensorUpdateInterval = pIntervalInSeconds;
}

bool GeneralSettings::GetEnablePowerSave() {
    return _enablePowerSave;
}

void GeneralSettings::SetEnablePowerSave(bool pEnable) {
    _enablePowerSave = pEnable;
}