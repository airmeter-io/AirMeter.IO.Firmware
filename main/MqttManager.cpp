#include "MqttManager.h"
#include "Utility.h"

#include "Json.h"

class ReadingsTopic : public MqttPublishTopic {
    std::string _topicName;
    std::string _currentValue;
    ValueModel& _values;
public:
    ReadingsTopic(GeneralSettings& pSettings, ValueModel& pValueModel) : _values(pValueModel) {
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
        if(_values.CO2) {
            json.CreateNumberProperty("CO2", _values.CO2->GetPPM());    
            json.CreateBoolProperty("CO2Heating", _values.CO2->GetIsHeatingUp());
            json.CreateBoolProperty("CO2Error", _values.CO2->GetHasError());                 
        }
        
        json.CreateStringProperty("Temp", _values.Bme280.GetTemperatureStr());
        json.CreateStringProperty("Pressure", _values.Bme280.GetPressureStr(2));
        json.CreateStringProperty("Humidity", _values.Bme280.GetHumidityStr()); 
        json.CreateStringProperty("Time", GetCurrentIsoTimeString()); 
        _currentValue = json.Print();
        return _currentValue;
    }
};

class InfoTopic : public MqttPublishTopic {
    std::string _topicName;
    std::string _currentValue;
    ValueModel& _values;
public:
    InfoTopic(GeneralSettings& pSettings, ValueModel& pValueModel) : _values(pValueModel) {
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
        if(_values.CO2) {            
            json.CreateStringProperty("CO2Name", _values.CO2->GetDeviceName().c_str()); 
            json.CreateStringProperty("CO2SerialNo", _values.CO2->GetSerialNo().c_str());             
        }
        json.CreateStringProperty("Time", GetCurrentIsoTimeString()); 
        _currentValue = json.Print();
        return _currentValue;
    }
};


MqttManager::MqttManager(GeneralSettings& pSettings, ValueModel& pValueModel) : _settings(pSettings), _valueModel(pValueModel)  {
    _mqtt = new Mqtt(pSettings.GetMqttServerAddress());
    _readings = new ReadingsTopic(pSettings, pValueModel);
    _info = new InfoTopic(pSettings, pValueModel);
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