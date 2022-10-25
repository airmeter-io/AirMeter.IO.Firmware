#pragma once


#include <vector>
#include "HttpRequest.h"

#include "Json.h"


class HttpJsonCommand  {
    public:
        virtual void Process(Json& pJson, Json& pResult ) = 0;
        virtual std::string GetName() = 0;
        virtual void ProcessFullResponse(Json& pJson, HttpRequest& pReq );
};

class HttpUrlHandler {  
     std::vector<httpd_uri_t*> _handlers;

     std::vector<HttpJsonCommand*> _commands;
protected:
    void RegisterCommand(HttpJsonCommand *pCommand);
    void ProcessAsJsonRequest(HttpRequest* pReq);
    virtual void ProcessRequest(HttpRequest *pReq)  = 0;
public:
    HttpUrlHandler(const std::vector<const char *> pPaths, httpd_method_t pMethod);
    ~HttpUrlHandler();

    std::vector<httpd_uri_t*>& GetHandlers();

    void Process(HttpRequest *pReq);
};


