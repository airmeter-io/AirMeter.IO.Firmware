#include "HttpRequest.h"

#define TAG "HttpRequest"

HttpRequest::HttpRequest(httpd_req_t *pReq) : _req(pReq) {

}

HttpRequest::~HttpRequest() {

}

std::string HttpRequest::GetUri() {
    return _req->uri;
}

std::string HttpRequest::GetHost() {
    auto len = httpd_req_get_hdr_value_len(_req, "Host") + 1;
    if (len > 1) {
        auto buf = (char *)malloc(len);
        std::string result;
        
        if (httpd_req_get_hdr_value_str(_req, "Host", buf, len) == ESP_OK) {
            result = buf;
        }
        free(buf);
        return result;
    }

    return "";
}

std::string HttpRequest::GetQuery() {
    auto len = httpd_req_get_url_query_len(_req) + 1;
    std::string result = "";
    if (len > 1) {
        auto buf = (char *)malloc(len);
        if (httpd_req_get_url_query_str(_req, buf, len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query1=%s", param);
            }
            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query3=%s", param);
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query2=%s", param);
            }
            result = buf;
        }
        free(buf);
    }
    return result;
}

void HttpRequest::SetType(const char* pContentType) {
    auto err = httpd_resp_set_type(_req, pContentType);
    if(err!=ESP_OK)
       ESP_LOGI(TAG, "ERROR Sending Response %d", err); 
}

void HttpRequest::SetHeader(const char *pHeader, const char *pValue) {
    auto err = httpd_resp_set_hdr(_req, pHeader,pValue);
    if(err!=ESP_OK)
       ESP_LOGI(TAG, "ERROR Sending Response %d", err); 
}

void HttpRequest::SendResponse(const std::string& pResponseText) {
    auto err = httpd_resp_send(_req,pResponseText.c_str(), pResponseText.length());
    if(err!=ESP_OK)
       ESP_LOGI(TAG, "ERROR Sending Response %d", err); 
}

void HttpRequest::SendResponse(const std::string& pContentType,const std::string& pResponseText) {
    httpd_resp_set_type(_req, pContentType.c_str());
    
    auto err = httpd_resp_send(_req,pResponseText.c_str(), pResponseText.length());
    if(err!=ESP_OK)
       ESP_LOGI(TAG, "ERROR Sending Response %d", err); 
}

void HttpRequest::SendChunk(const char* pContent, const size_t pLength) {
    auto err = httpd_resp_send_chunk(_req,pContent, pLength);
    if(err!=ESP_OK)
       ESP_LOGI(TAG, "ERROR Sending Chunk %d", err); 
}

void HttpRequest::SendChunk(const std::string& pChunk) {
    SendChunk(pChunk.c_str(), pChunk.size());
}

void HttpRequest::FinishedChunks() {
    auto err = httpd_resp_send_chunk(_req,0, 0);
    if(err!=ESP_OK)
       ESP_LOGI(TAG, "ERROR Finishing Chunks %d", err);     
}


void HttpRequest::Redirect(const std::string& pDestination) {
    httpd_resp_set_status(_req, "302 Found");
    httpd_resp_set_hdr(_req, "Location", pDestination.c_str());
    httpd_resp_send(_req,"", 0);
}
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
std::string HttpRequest::GetRequestContentAsString() {
    char buf[101];
    int ret, remaining = _req->content_len;
    std::string result;
    while (remaining > 0) {
        if ((ret = httpd_req_recv(_req, buf,
                        MIN(remaining, sizeof(buf)-1))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            return "Error";
        }
       
        remaining -= ret;
        buf[ret] = 0;
        result+=buf;        
    }
    return result;
}
