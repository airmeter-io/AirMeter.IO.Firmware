#pragma once

#include "Common.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>



class MqttPublishTopic {

    public:
        virtual int GetQOS();
        virtual int GetRetain();
        virtual std::string& GetTopicName() = 0;
        virtual std::string& GetCurrentValue() = 0;
        virtual void NotifyPublished();
        ~MqttPublishTopic() {}
};

class MqttInternal {
    public:
       virtual void ProcessEvent(esp_mqtt_event_handle_t pEvent) = 0;
    
};

class Mqtt : private MqttInternal {
    esp_mqtt_client_config_t _clientConfig;
    esp_mqtt_client_handle_t _client = {};
    bool _connected = false;

    std::vector<MqttPublishTopic*> _publishedTopics;
    std::map<MqttPublishTopic*,int> _publishedTopicsMsgIds;
    void ProcessEvent(esp_mqtt_event_handle_t pEvent) override;
public:
    Mqtt(const std::string& pServerAddress, const std::string& pUsername, const std::string& pPassword);
    ~Mqtt();

    void Publish(MqttPublishTopic* pTopic);
};