#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "DataManager.h"
#include "Json.h"

class GetLatestDataCommand : public HttpJsonCommand  {
    
    public:
        GetLatestDataCommand();
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
