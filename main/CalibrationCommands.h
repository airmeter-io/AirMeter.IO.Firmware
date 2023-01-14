#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "Wifi.h"

#include "Json.h"


class GetCalibrationInfoCommand : public HttpJsonCommand  {

    public:
        GetCalibrationInfoCommand() ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};

class ManualCalibrationCommand : public HttpJsonCommand  {
    public:
        ManualCalibrationCommand() ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};

class EnableAbcCommand : public HttpJsonCommand  {
    public:
        EnableAbcCommand() ;
        void Process(Json& pJson,Json& pResult) override;
        std::string GetName() override ;
};

class DisableAbcCommand : public HttpJsonCommand  {
    public:
        DisableAbcCommand() ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};