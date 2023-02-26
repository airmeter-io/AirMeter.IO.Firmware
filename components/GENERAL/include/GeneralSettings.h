#pragma once

#include "Common.h"
#include <string>
#include <vector>
#include "Json.h"
#include "SettingsBase.h"

enum CO2SensorType { None, MHZ19, Cubic, SenseAirModBus, MaxType };


class GeneralSettings : public SettingsBase {

   std::string _deviceName = "device";
   std::string _apPassword = "";
   std::string _mqttServerAddress = "";
   std::string _mqttTopic = "CO2Monitors/%DEVICE_NAME%";
   std::string _primaryNtpServer = "pool.ntp.org";
   std::string _secondaryNtpServer = "";
   CO2SensorType _sensorType =  CO2SensorType::None;
   int _sensorUpdateInterval = 10;
   bool _enableMqtt  = false;
   bool _enableDhcpNtp = false;
   int _mqttPublishSecondDelay = 5*60;
   bool _enablePowerSave = true;
   int _backgroundCO2 = 400;

protected:
    const char* GetFilePath() override;
    void LoadSettingsFromJson(Json& pJson) override;
    void SaveSettingsToJson(Json& pJson) override;
public:
    GeneralSettings();
    
    std::string GetDeviceName();
    void SetDeviceName(const std::string& pDeviceName);
    std::string GetApPassword();
    void SetApPassword(const std::string& pPassword);
    std::string& GetMqttServerAddress();
    void SetMqttServerAddress(const std::string& pAddress);
    std::string GetMqttTopic();
    void SetMqttTopic(const std::string& pTopic);
    bool GetEnableMqtt();
    void SetEnableMqtt(bool pValue);
    int GetMqttPublishDelay();
    void SetMqttPublishDelay(int pDelayInSeconds);
    bool GetEnableDhcpNtp();
    void SetEnableDhcpNtp(bool pValue);
    std::vector<std::string*> GetNtpServers();
    void SetPrimaryNtpServer(const std::string &pServer);
    void SetSecondaryNtpServer(const std::string &pServer);
    std::string GetPrimaryNtpServer();
    std::string GetSecondaryNtpServer();
    CO2SensorType GetCO2SensorType();
    void SetCO2SensorType(CO2SensorType pSensorType);
    int GetSensorUpdateInterval();
    void SetSensorUpdateInterval(int pIntervalInSeconds);
    bool GetEnablePowerSave();
    void SetEnablePowerSave(bool pEnable);
    int GetBackgroundCO2();
    void SetBackgroundCO2(int pBaseline);
};