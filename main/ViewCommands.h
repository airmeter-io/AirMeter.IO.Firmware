#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "DataManagerStore.h"
#include "Json.h"

class GetLatestDataCommand : public HttpJsonCommand  {
    
    public:
        GetLatestDataCommand();
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};

class GetHistoricalDataCommand : public HttpJsonCommand  {
    DataManagerStore& _manager;
    public:
        GetHistoricalDataCommand(DataManagerStore& pManager);
        void Process(Json& pJson,Json& pResult) override;
        void ProcessFullResponse(Json& pJson, HttpRequest& pReq ) override;
        std::string GetName() override;
};
