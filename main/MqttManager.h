#pragma once

#include "Common.h"
#include "Mqtt.h"
#include "GeneralSettings.h"
#include "Json.h"


class ReadingsTopic;
class InfoTopic;

class MqttManager {
    private:
        Mqtt* _mqtt;
        GeneralSettings& _settings;
        ReadingsTopic* _readings;
        InfoTopic* _info;
        time_t _lastPublish;
    public:
        MqttManager(GeneralSettings& pSettings);
        ~MqttManager();

        void Tick();
};