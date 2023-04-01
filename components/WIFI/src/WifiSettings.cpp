#include "WifiSettings.h"

const char* WifiSettings::GetFilePath() {
    return "/spiffs/wireless.json";
}

void WifiSettings::LoadSettingsFromJson(Json& pJson) {
    if(pJson.HasArrayProperty("Networks")) {
        auto networksProp = pJson.GetObjectProperty("Networks");
        std::vector<Json*> networkElements;
        networksProp->GetAsArrayElements(networkElements);
        for(auto json : networkElements) {
            if(json->HasProperty("Ssid") && json->HasProperty("Password") && json->HasProperty("AuthMode") && json->HasProperty("Priority")) {
                auto network = new WifiConfiguredNetwork();

                network->ssid = json->GetStringProperty("Ssid");
                network->password = json->GetStringProperty("Password");
                network->authMode = json->GetStringProperty("AuthMode");
                network->priority = (uint32_t)json->GetIntProperty("Priority");
                if(_networks[network->ssid]!=nullptr) delete _networks[network->ssid];
                _networks[network->ssid] =  network;                
            }
            delete json;        
        }

        delete networksProp;
    }    
}

void WifiSettings::SaveSettingsToJson(Json& pJson) {
    std::vector<Json*> networkElements;
    for(auto network : _networks) {
        auto networkJson = new Json();
        networkJson->CreateStringProperty("Ssid",network.second->ssid);
        networkJson->CreateStringProperty("Password", network.second->password);
        networkJson->CreateStringProperty("AuthMode", network.second->authMode);
        networkJson->CreateNumberProperty("Priority", (int)network.second->priority);
        networkElements.push_back(networkJson);
    }
    pJson.CreateArrayProperty("Networks", networkElements);
}

WifiSettings::WifiSettings() {
    Load();
}
