#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "Wifi.h"
#include "ValueModel.h"
#include "Json.h"


class GetCalibrationInfoCommand : public HttpJsonCommand  {
    ValueModel& _values;
    public:
        GetCalibrationInfoCommand(ValueModel& pValues) ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};

class ManualCalibrationCommand : public HttpJsonCommand  {
    ValueModel& _values;
    public:
        ManualCalibrationCommand(ValueModel& pValues) ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};

class EnableAbcCommand : public HttpJsonCommand  {
    ValueModel& _values;
    public:
        EnableAbcCommand(ValueModel& pValues) ;
        void Process(Json& pJson,Json& pResult) override;
        std::string GetName() override ;
};

class DisableAbcCommand : public HttpJsonCommand  {
    ValueModel& _values;
    public:
        DisableAbcCommand(ValueModel& pValues) ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};