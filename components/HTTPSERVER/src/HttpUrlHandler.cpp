#include "HttpUrlHandler.h"
#include "Utility.h"

#define TAG "HttpUrlHandler"

extern "C" {
    #include <stdio.h>
    #include <assert.h>
    #include "esp_log.h"
    #include <esp_http_server.h>
}

esp_err_t handler(httpd_req_t *pReq) {
    auto handler = (HttpUrlHandler*)pReq->user_ctx;
    HttpRequest req(pReq);
    handler->Process(&req);
    return ESP_OK;
}



HttpUrlHandler::HttpUrlHandler(const std::vector<const char *> pPaths, httpd_method_t pMethod) {
    for(auto path : pPaths) {
        auto uri = (httpd_uri_t *)calloc(1,sizeof(httpd_uri_t));
        uri->uri = path;
        uri->method = pMethod;
        uri->handler = handler;
        uri->user_ctx = this;
        _handlers.push_back( uri );
    }
}

HttpUrlHandler::~HttpUrlHandler() {

}

void HttpUrlHandler::RegisterCommand(HttpJsonCommand *pCommand) {
    _commands.push_back(pCommand);
}

void HttpUrlHandler::ProcessAsJsonRequest(HttpRequest* pReq) {
    auto jsonStr = pReq->GetRequestContentAsString();
    Json json(jsonStr);
    

    auto commandName = json.GetStringProperty("COMMAND");
    printf("Processing Command %s\n", commandName.c_str());
    
    for(auto command : _commands)
        if(commandName == command->GetName()) {
            command->ProcessFullResponse(json, *pReq);
            return; 
        }

    pReq->SendResponse("{ \"Status\": \"false\"}");
}


std::vector<httpd_uri_t *>& HttpUrlHandler::GetHandlers() {
    return _handlers;
}

void HttpUrlHandler::Process(HttpRequest *pReq) {
    ProcessRequest(pReq);
}

void HttpJsonCommand::ProcessFullResponse(Json& pJson, HttpRequest& pReq ) {
    Json result;
    Process(pJson,result);
    pReq.SetHeader("Access-Control-Allow-Origin", "*");
    pReq.SendResponse("application/json;charset=UTF-8",result.Print());
}