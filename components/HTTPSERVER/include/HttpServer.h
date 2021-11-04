#pragma once
#include <vector>
#include <string>
#include "Common.h"
#include "HttpUrlHandler.h"


class HttpServer {  
    httpd_handle_t _server = NULL;
    httpd_config_t _config;
    std::vector<HttpUrlHandler*> _handlers;
    bool _started = false;
public:
    HttpServer();
    ~HttpServer();

    void AddUrlHandler(HttpUrlHandler *pHandler);
    void RemoveUrlHandler(HttpUrlHandler *pHandler);

    void Start();
    void Stop();
};