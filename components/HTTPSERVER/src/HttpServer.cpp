#include "HttpServer.h"
#include <algorithm>
#define TAG "HTTP"


HttpServer::HttpServer() {
}


HttpServer::~HttpServer() {

}

void HttpServer::AddUrlHandler(HttpUrlHandler *pHandler) {
    _handlers.push_back(pHandler);
    if(_started)
        for(auto httpHandler : pHandler->GetHandlers())
            httpd_register_uri_handler(_server, httpHandler);
}


void HttpServer::RemoveUrlHandler(HttpUrlHandler *pHandler) {
    _handlers.erase(std::remove(_handlers.begin(), _handlers.end(), pHandler), _handlers.end());
    if(_started) 
        for(auto httpHandler : pHandler->GetHandlers())
            httpd_unregister_uri_handler(_server, httpHandler->uri, httpHandler->method);
}


void HttpServer::Start() {
    _config = HTTPD_DEFAULT_CONFIG();
    _config.max_open_sockets = 13;
    _config.max_uri_handlers += 10;
    ESP_LOGI(TAG, "Starting server on port: '%d'", _config.server_port);
    if (httpd_start(&_server, &_config) == ESP_OK) {   
        ESP_LOGI(TAG, "Registering URI handlers");
        for(auto handler : _handlers)
            for(auto httpHandler : handler->GetHandlers())
                httpd_register_uri_handler(_server, httpHandler);
        _started = true;
    } 
    else
         ESP_LOGI(TAG, "Error Starting HTTP Server");
}

void HttpServer::Stop() {
    httpd_stop(_server);
}
