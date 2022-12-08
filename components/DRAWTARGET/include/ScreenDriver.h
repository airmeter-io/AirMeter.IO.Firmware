
#pragma once

#include "Json.h"
#include "DrawTarget.h"


class ScreenDriver  {
    public:
        virtual DrawControl* LoadDriver(Json* pConfiguration ) = 0;
        virtual std::string GetName() const = 0;     
};