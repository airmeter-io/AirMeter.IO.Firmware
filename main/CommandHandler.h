#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "Wifi.h"
#include "ValueModel.h"
#include "Json.h"
#include "GeneralSettings.h"
#include "DataManager.h"



class CommandHandler : public HttpUrlHandler {  
private:
    Wifi& _wifi;
    ValueModel& _values;
    GeneralSettings& _settings;
    
public:
    CommandHandler(Wifi& pWifi, GeneralSettings& pSettings, ValueModel& pValues, DataManager& pDataManager);
    ~CommandHandler();

    void ProcessRequest(HttpRequest *pReq) override;
};