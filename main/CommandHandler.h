#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "Wifi.h"
#include "Json.h"
#include "GeneralSettings.h"
#include "DataManager.h"



class CommandHandler : public HttpUrlHandler {  
private:
    Wifi& _wifi;
    GeneralSettings& _settings;
    
public:
    CommandHandler(Wifi& pWifi, GeneralSettings& pSettings, DataManager& pDataManager);
    ~CommandHandler();

    void ProcessRequest(HttpRequest *pReq) override;
};