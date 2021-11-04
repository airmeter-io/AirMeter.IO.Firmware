#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "Wifi.h"
#include "ValueModel.h"
#include "DataManager.h"
#include "Json.h"

class GetLatestDataCommand : public HttpJsonCommand  {
    ValueModel& _values;
    public:
        GetLatestDataCommand(ValueModel& pValues);
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};

class GetHistoricalDataCommand : public HttpJsonCommand  {
    DataManager& _manager;
    public:
        GetHistoricalDataCommand(DataManager& pManager);
        void Process(Json& pJson,Json& pResult) override;
        void ProcessFullResponse(Json& pJson, HttpRequest& pReq ) override;
        std::string GetName() override;
};
