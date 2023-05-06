#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "WifiTask.h"
#include "Json.h"
#include "GeneralSettings.h"
#include "DataManagerStore.h"
#include "MqttManager.h"


class CommandHandler : public HttpUrlHandler {  
private:
    WifiTask& _wifi;
    GeneralSettings& _settings;
    
public:
    CommandHandler(WifiTask& pWifi, GeneralSettings& pSettings, DataManagerStore& pDataManager, MqttManager& pMqttManager);
    ~CommandHandler();

    void ProcessRequest(HttpRequest *pReq) override;
};