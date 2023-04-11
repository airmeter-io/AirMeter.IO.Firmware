#include "Mqtt.h"

extern "C" {
    #include "esp_crt_bundle.h"
}

static const char *TAG = "MQTT";


int MqttPublishTopic::GetQOS() {
    return 1;
}

int MqttPublishTopic::GetRetain() {
    return 0;
}

void MqttPublishTopic::NotifyPublished() {

}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    auto eventData = (esp_mqtt_event_handle_t)event_data;
    auto mqtt = (MqttInternal*)(handler_args);
    mqtt->ProcessEvent(eventData);
}

Mqtt::Mqtt(const std::string& pServerAddress, const std::string& pUsername, const std::string& pPassword) {
    memset(&_clientConfig, 0, sizeof(_clientConfig));
    _clientConfig.broker.address.uri = pServerAddress.c_str();   
    _clientConfig.broker.verification.crt_bundle_attach=esp_crt_bundle_attach;
    if(pUsername.size()>0)
        _clientConfig.credentials.username = pUsername.c_str();
    if(pPassword.size()>0)
        _clientConfig.credentials.authentication.password = pPassword.c_str();
    _client = esp_mqtt_client_init(&_clientConfig);
    esp_mqtt_client_register_event(_client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, this);
    esp_mqtt_client_start(_client);
}

Mqtt::~Mqtt() {

}

void Mqtt::Publish(MqttPublishTopic* pTopic) {
    if(std::count(_publishedTopics.begin(), _publishedTopics.end(), pTopic) == 0) {
        _publishedTopics.push_back(pTopic);
        _publishedTopicsMsgIds.insert(std::pair<MqttPublishTopic*,int>(pTopic, 0));
    }

    if(_connected) {
        _publishedTopicsMsgIds.find(pTopic)->second = esp_mqtt_client_publish(_client, pTopic->GetTopicName().c_str(), pTopic->GetCurrentValue().c_str(), 0, pTopic->GetQOS(), pTopic->GetRetain());
    }
}

void Mqtt::ProcessEvent(esp_mqtt_event_handle_t pEvent) {
    switch (pEvent->event_id) {
        case MQTT_EVENT_CONNECTED:            
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            _connected = true;
            for(auto topic : _publishedTopics) {
                Publish(topic);
            }
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", pEvent->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", pEvent->msg_id);
            for(auto topicAndMsgId : _publishedTopicsMsgIds) {
                if(topicAndMsgId.second == pEvent->msg_id) {
                    topicAndMsgId.first->NotifyPublished();
                }
            }
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", pEvent->topic_len, pEvent->topic);
            printf("DATA=%.*s\r\n", pEvent->data_len, pEvent->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", pEvent->event_id);
            break;
    }
}