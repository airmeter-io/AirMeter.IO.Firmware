#include "CaptiveRedirectHandler.h"
#include "Json.h"



CaptiveRedirectHandler::CaptiveRedirectHandler() : HttpUrlHandler({"/redirect","/connecttest.txt", "/generate_204", "/nsci.txt" }, HTTP_GET) {

}


CaptiveRedirectHandler::~CaptiveRedirectHandler() {

}


void CaptiveRedirectHandler::ProcessRequest(HttpRequest *pReq) {
    printf("Got request to redirect: HOST=%s, QUERY=%s\n", pReq->GetHost().c_str(), pReq->GetQuery().c_str());
    pReq->Redirect("http://captive.portal/");
}

