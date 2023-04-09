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
    ReadingsTopic(GeneralSettings& pSettings)  {
        _topicName = pSettings.GetMqttTopic();

        ReplaceAll(_topicName, "%DEVICE_NAME%", pSettings.GetDeviceName());
        _topicName += "/SENSOR";
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
    InfoTopic(GeneralSettings& pSettings)  {
        _topicName = pSettings.GetMqttTopic();
        ReplaceAll(_topicName, "%DEVICE_NAME%", pSettings.GetDeviceName());
        _topicName += "/INFO";
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
    _mqtt = new Mqtt(pSettings.GetMqttServerAddress());
    _readings = new ReadingsTopic(pSettings);
    _info = new InfoTopic(pSettings);
    _mqtt->Publish(_readings);
    _mqtt->Publish(_info);
    time(&_lastPublish);
}

MqttManager::~MqttManager() {
    //delete _mqtt;
    //delete _readings;
}


void MqttManager::Tick() {
    time_t now;
    time(&now);
    if(now - _lastPublish >= _settings.GetMqttPublishDelay()) {
        _mqtt->Publish(_readings);
        _lastPublish = now;
    }
}