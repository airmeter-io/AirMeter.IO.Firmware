#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "GeneralSettings.h"
#include "Json.h"


class CalibrationCommand : public HttpJsonCommand  {

    private:
        GeneralSettings& _settings;
    public:
        CalibrationCommand(GeneralSettings& pSettings) ;
        void Process(Json& pJson,Json& pResult) override ;
        std::string GetName() override ;
};

