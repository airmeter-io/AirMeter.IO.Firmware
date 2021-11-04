#include "Json.h"
#include "JsonArray.h"

#define TAG "Json"
Json::Json() {
    _json = cJSON_CreateObject();
}

Json::Json(std::string pJson) {
    _json = cJSON_Parse(pJson.c_str());
    ESP_LOGI(TAG, "Registering URI handlers");
    if (_json == NULL)
    {
       ESP_LOGI(TAG, "Coult not parse JSON");
    }
}



Json::~Json() {
    if(!_dontFree)
        cJSON_Delete(_json);
}

std::string Json::GetStringProperty(const std::string& pProperty) {
    auto result= cJSON_GetObjectItemCaseSensitive(_json, pProperty.c_str());
    if (cJSON_IsString(result) && (result->valuestring != NULL))
        return result->valuestring;  
    
     ESP_LOGI(TAG, "Coult not find JSON Property");
     return "";
}


const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

bool Json::GetBoolProperty(const std::string& pProperty) {
    auto resultStr = trim(GetStringProperty(pProperty));
    return resultStr.compare("true") == 0;
}

int Json::GetIntProperty(const std::string& pProperty) {
    auto result= cJSON_GetObjectItemCaseSensitive(_json, pProperty.c_str());
    if(cJSON_IsString(result) && (result->valuestring != NULL)) {
        auto resultStr = trim(result->valuestring);
        return atoi(resultStr.c_str());
    } else if (cJSON_IsNumber(result)) {
        return result->valueint;
    }
    return 0;    
}

uint Json::GetUIntProperty(const std::string& pProperty) {
    auto result= cJSON_GetObjectItemCaseSensitive(_json, pProperty.c_str());
    if(cJSON_IsString(result) && (result->valuestring != NULL)) {
        auto resultStr = trim(result->valuestring);
        return strtoul (resultStr.c_str(), NULL, 0);
    } else if (cJSON_IsNumber(result)) {
        return result->valueint;
    }
    return 0;    
}

bool Json::HasProperty(const std::string& pProperty) {
    auto result= cJSON_GetObjectItemCaseSensitive(_json, pProperty.c_str());
    if ((cJSON_IsString(result) && (result->valuestring != NULL)) || cJSON_IsNumber(result) || cJSON_IsBool(result))
        return true;    
    return false;
}

void Json::CreateNumberProperty(const std::string& pProperty, double pValue) {
    CreateStringProperty(pProperty, std::to_string(pValue)); 
}

void Json::CreateStringProperty(const std::string& pProperty, const std::string& pValue) {
    cJSON_AddStringToObject(_json, pProperty.c_str(), pValue.c_str());
}

void Json::CreateNumberProperty(const std::string& pProperty, int pValue) {
    CreateStringProperty(pProperty, std::to_string(pValue));
}

void Json::CreateBoolProperty(const std::string& pProperty, bool pValue) {
   CreateStringProperty(pProperty, pValue? "true" : "false");
}

void Json::CreateArrayProperty(const std::string& pProperty, std::vector<Json*>& pElements) {
    JsonArray array;
    for(auto element : pElements) {
        element->_dontFree = true;
        array.Add(*element);
        delete element;
    }

    cJSON_AddItemToObject(_json,pProperty.c_str(),array.GetJsonObject());
}

std::string Json::Print() {
    auto jsonAsCStr = cJSON_Print(_json);
    std::string result = jsonAsCStr;
    free(jsonAsCStr);
    return result;
}

cJSON* Json::GetJsonObject() {
    return _json;
}
