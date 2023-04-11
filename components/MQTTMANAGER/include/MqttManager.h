#pragma once

#include "Common.h"
#include "Mqtt.h"
#include "GeneralSettings.h"
#include "Json.h"


class ReadingsTopic;
class InfoTopic;

class MqttManager : public SettingsBase{
private:
    Mqtt* _mqtt = nullptr;
    GeneralSettings& _settings;
    ReadingsTopic* _readings = nullptr;
    InfoTopic* _info = nullptr;
    time_t _lastPublish;
    std::string _serverAddress = "";
    std::string _username = "";
    std::string _password = "";
    std::string _readingsTopic = "CO2Monitors/%DEVICE_NAME%/SENSOR";
    std::string _infoTopic = "CO2Monitors/%DEVICE_NAME%/INFO";
    bool _enable  = false;
    int _publishSecondDelay = 5*60;
protected:
    const char* GetFilePath() override;
    void LoadSettingsFromJson(Json& pJson) override;
    void SaveSettingsToJson(Json& pJson) override;
public:
    MqttManager(GeneralSettings& pSettings);
    ~MqttManager();

   
    void Tick();

    
    std::string& GetServerAddress();
    void SetServerAddress(const std::string& pAddress);
    std::string& GetUsername();
    void SetUsername(const std::string& pUsername);
    std::string& GetPassword();
    void SetPassword(const std::string& pPassword);

    std::string GetReadingsTopic();
    void SetReadingsTopic(const std::string& pTopic);
    std::string GetInfoTopic();
    void SetInfoTopic(const std::string& pTopic);
    bool GetEnable();
    void SetEnable(bool pValue);
    int GetPublishDelay();
    void SetPublishDelay(int pDelayInSeconds);

    
};