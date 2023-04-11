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

   std::string _primaryNtpServer = "pool.ntp.org";
   std::string _secondaryNtpServer = "";
   CO2SensorType _sensorType =  CO2SensorType::None;
   int _sensorUpdateInterval = 60;

   bool _enableDhcpNtp = false;
  
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