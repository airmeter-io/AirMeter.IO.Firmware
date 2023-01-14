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

        for(auto keyPair : ValueController::GetCurrent().GetSourcesByName()) {
            auto source = keyPair.second->DefaultSource;
            if(source->IsIncludedInMQTTReadings() ) {
                source->SerialiseToJsonProperty(json);
            }            
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
        
        auto deviceNameSource = ValueController::GetCurrent().GetDefault(CO2VALUE_DEVICENAME);              
        auto serialNoSource = ValueController::GetCurrent().GetDefault(CO2VALUE_SERIALNO);     

        if(deviceNameSource)           
            json.CreateStringProperty("CO2Name",  deviceNameSource->GetValue().s->c_str()); 

        if(serialNoSource)
            json.CreateStringProperty("CO2SerialNo", serialNoSource->GetValue().s->c_str());             
        
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