#pragma once

#include "Common.h"
#include "HttpUrlHandler.h"
#include<vector>
#include<string>

class WebContentHandler : public HttpUrlHandler {  
private:
    static bool _loaded ;
    static std::vector<const char *> _files;
    static std::vector<const char *> _webNames;
    static std::vector<const char *> GetWebFiles();
public:
    WebContentHandler();
    ~WebContentHandler();

     void ProcessRequest(HttpRequest *pReq) override;
};
