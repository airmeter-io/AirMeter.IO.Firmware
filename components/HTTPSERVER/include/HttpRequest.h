#pragma once

#include <string>

extern "C" {
    #include <stdio.h>
    #include <assert.h>
    #include "esp_log.h"
    #include <esp_http_server.h>

}

class HttpRequest {  
    httpd_req_t *_req;
public:
    HttpRequest(httpd_req_t *pReq);
    ~HttpRequest();
    std::string GetHost();
    std::string GetQuery();
    std::string GetUri();
    void SetType(const char* pContentType);
    void SetHeader(const char* pHeader, const char* pValue);
    void SendResponse(const std::string& pResponseText);
    void SendResponse(const std::string& pContentType,const std::string& pResponseText);
    void SendChunk(const char* pContent, const size_t pLength);
    void SendChunk(const std::string& pChunk);
    void FinishedChunks();
    void Redirect(const std::string& pDestination);
    std::string GetRequestContentAsString();
};