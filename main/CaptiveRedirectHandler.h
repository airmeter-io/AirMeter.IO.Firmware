#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include "Wifi.h"


class CaptiveRedirectHandler : public HttpUrlHandler {  
public:
    CaptiveRedirectHandler();
    ~CaptiveRedirectHandler();

     void ProcessRequest(HttpRequest *pReq) override;
};