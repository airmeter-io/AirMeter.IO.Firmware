#include "MqttManager.h"
#include "Utility.h"
#include "CO2Sensor.h"
#include "ValueController.h"
#include "CommonValueNames.h"
#include "Json.h"

class ReadingsTopic : public MqttPublishTopic {
    std::string _topicName;
    std::string _currentValue;

public:
    ReadingsTopic(MqttManager& pMqttManager, GeneralSettings& pSettings)  {
        _topicName = pMqttManager.GetReadingsTopic();

        ReplaceAll(_topicName, "%DEVICE_NAME%", pSettings.GetDeviceName());
    }

    ~ReadingsTopic() {
        
    }

    std::string& GetTopicName() {
        return _topicName;
    }
    std::string& GetCurrentValue() {
        Json json;

        for(auto groupPair : ValueController::GetCurrent().GetGroups()) {
            Json* groupJson = nullptr;
            for(auto keyPair : groupPair.second->SourcesByName) {
                auto source = keyPair.second->DefaultSource;
                if(source->IsIncludedInMQTTReadings() ) {
                    if(groupJson == nullptr) 
                        groupJson = json.CreateObjectProperty(groupPair.first);
                    source->SerialiseToJsonProperty(*groupJson);
                }            
            }   
            if(groupJson!=nullptr)
                delete groupJson; 
        } 
       

        json.CreateStringProperty("Time", GetCurrentIsoTimeString()); 
        _currentValue = json.Print();
        return _currentValue;
    }
};

class InfoTopic : public MqttPublishTopic {
    std::string _topicName;
    std::string _currentValue;
public:
    InfoTopic(MqttManager& pMqttManager, GeneralSettings& pSettings)  {
        _topicName = pMqttManager.GetInfoTopic();
        ReplaceAll(_topicName, "%DEVICE_NAME%", pSettings.GetDeviceName());
    }

    ~InfoTopic() {
        
    }



    std::string& GetTopicName() {
        return _topicName;
    }
    std::string& GetCurrentValue() {
        Json json;
        
        for(auto groupPair : ValueController::GetCurrent().GetGroups()) {
            Json* groupJson = nullptr;
            for(auto keyPair : groupPair.second->SourcesByName) {
                auto source = keyPair.second->DefaultSource;
                if(source->IsIncludedInMQTTInfo() ) {
                    if(groupJson == nullptr) 
                        groupJson = json.CreateObjectProperty(groupPair.first);
                    source->SerialiseToJsonProperty(*groupJson);
                }            
            }   
            if(groupJson!=nullptr)
                delete groupJson; 
        }             
        
        json.CreateStringProperty("Time", GetCurrentIsoTimeString()); 
        _currentValue = json.Print();
        return _currentValue;
    }
};


MqttManager::MqttManager(GeneralSettings& pSettings) : _settings(pSettings)  {
    Load();
    if(GetEnable()) {
        _mqtt = new Mqtt(GetServerAddress(), _username, _password );
        _readings = new ReadingsTopic(*this, pSettings);
        _info = new InfoTopic(*this, pSettings);
        _mqtt->Publish(_readings);
        _mqtt->Publish(_info);
    }
    time(&_lastPublish);
}

MqttManager::~MqttManager() {
    //delete _mqtt;
    //delete _readings;
}


void MqttManager::Tick() {
    if(_mqtt!=nullptr && _enable) {
        time_t now;
        time(&now);
        if(now - _lastPublish >= GetPublishDelay()) {
            printf("Publishing\n");
            _mqtt->Publish(_readings);
            _lastPublish = now;
        }
    }
}


const char * MqttManager::GetFilePath() {
    return "/spiffs/mqtt.json";
}

void MqttManager::LoadSettingsFromJson(Json& pJson) {     
    if(pJson.HasProperty("Enable"))
        _enable = pJson.GetBoolProperty("Enable");
    if(pJson.HasProperty("Server"))
        _serverAddress = pJson.GetStringProperty("Server");
    if(pJson.HasProperty("Username"))
        _username = pJson.GetStringProperty("Username");
    if(pJson.HasProperty("Password"))
        _password = pJson.GetStringProperty("Password");        
    if(pJson.HasProperty("Readings"))
        _readingsTopic = pJson.GetStringProperty("Readings");    
    if(pJson.HasProperty("Info"))
        _infoTopic = pJson.GetStringProperty("Info");            
    if(pJson.HasProperty("Delay"))
        _publishSecondDelay = pJson.GetIntProperty("Delay");           
}

void MqttManager::SaveSettingsToJson(Json& pJson) {    
    pJson.CreateBoolProperty("Enable", _enable);
    pJson.CreateStringProperty("Readings", _readingsTopic);
    pJson.CreateStringProperty("Info", _infoTopic);
    pJson.CreateNumberProperty("Delay", _publishSecondDelay);
    pJson.CreateStringProperty("Server", _serverAddress);   
    pJson.CreateStringProperty("Username", _username);   
    pJson.CreateStringProperty("Password", _password);   
}

std::string& MqttManager::GetServerAddress() {
  return _serverAddress;
}

void MqttManager::SetServerAddress(const std::string& pValue) {
   _serverAddress = pValue;
}

std::string& MqttManager::GetUsername() {
    return _username;
}

void MqttManager::SetUsername(const std::string& pUsername) {
    _username = pUsername;
}

std::string& MqttManager::GetPassword() {
    return _password;
}

void MqttManager::SetPassword(const std::string& pPassword) {
    _password = pPassword;
}

bool MqttManager::GetEnable() { 
    return _enable;
}

void MqttManager::SetEnable(bool pValue) {
    _enable = pValue;
}

int MqttManager::GetPublishDelay() {
    return _publishSecondDelay;
}

void MqttManager::SetPublishDelay(int pDelay) {
    _publishSecondDelay = pDelay;
}


std::string MqttManager::GetReadingsTopic() {
    return _readingsTopic;
}

void MqttManager::SetReadingsTopic(const std::string& pTopic) {
    _readingsTopic = pTopic;
}

std::string MqttManager::GetInfoTopic() {
    return _infoTopic;
}

void MqttManager::SetInfoTopic(const std::string& pTopic) {
    _infoTopic = pTopic;
}
