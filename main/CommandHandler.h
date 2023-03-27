#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "WifiTask.h"
#include "Json.h"
#include "GeneralSettings.h"
#include "DataManager.h"



class CommandHandler : public HttpUrlHandler {  
private:
    WifiTask& _wifi;
    GeneralSettings& _settings;
    
public:
    CommandHandler(WifiTask& pWifi, GeneralSettings& pSettings, DataManager& pDataManager);
    ~CommandHandler();

    void ProcessRequest(HttpRequest *pReq) override;
};