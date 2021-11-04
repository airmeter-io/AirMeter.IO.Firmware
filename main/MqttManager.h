#pragma once

#include "Common.h"
#include "Mqtt.h"
#include "GeneralSettings.h"
#include "Json.h"
#include "ValueModel.h"

class ReadingsTopic;
class InfoTopic;

class MqttManager {
    private:
        Mqtt* _mqtt;
        GeneralSettings& _settings;
        ValueModel& _valueModel;
        ReadingsTopic* _readings;
        InfoTopic* _info;
        time_t _lastPublish;
    public:
        MqttManager(GeneralSettings& pSettings, ValueModel& pValueModel);
        ~MqttManager();

        void Tick();
};