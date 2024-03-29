#include "Json.h"
#include "JsonArray.h"

#define TAG "Json"
Json::Json() {
    _json = cJSON_CreateObject();

}

Json::Json(cJSON* pJson) : _json(pJson) {
    _dontFree = true;
}

Json::Json(std::string pJson) {
    _json = cJSON_Parse(pJson.c_str());   
    if (_json == NULL)
    {
       ESP_LOGI(TAG, "Coult not parse JSON");
    }
}



Json::~Json() {
    if(!_dontFree)
        cJSON_Delete(_json);
}


Json* Json::GetObjectProperty(const std::string & pProperty) {
    cJSON * result= cJSON_GetObjectItemCaseSensitive(_json, pProperty.c_str());
    if(result!=nullptr) 
        return new Json(result);
    return nullptr;
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
    auto result= cJSON_GetObjectItemCaseSensitive(_json, pProperty.c_str());
    if(cJSON_IsBool(result))
        return cJSON_IsTrue(result);
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

bool Json::HasObjectProperty(const std::string& pProperty) {
    auto result= cJSON_GetObjectItemCaseSensitive(_json, pProperty.c_str());
    if (cJSON_IsObject(result))
        return true;    
    return false;
}

bool Json::HasArrayProperty(const std::string& pProperty) {
    auto result= cJSON_GetObjectItemCaseSensitive(_json, pProperty.c_str());
    if (cJSON_IsArray(result))
        return true;    
    return false;
}

void Json::CreateNumberProperty(const std::string& pProperty, double pValue) {
    cJSON_AddNumberToObject(_json, pProperty.c_str(), pValue);
}

void Json::CreateStringProperty(const std::string& pProperty, const std::string& pValue) {
    cJSON_AddStringToObject(_json, pProperty.c_str(), pValue.c_str());
}

void Json::CreateNumberProperty(const std::string& pProperty, int pValue) {
    cJSON_AddNumberToObject(_json, pProperty.c_str(), pValue);    
}

void Json::CreateBoolProperty(const std::string& pProperty, bool pValue) {
    cJSON_AddBoolToObject(_json, pProperty.c_str(), pValue);
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

Json* Json::CreateObjectProperty(const std::string& pProperty) {
    Json* json = new Json();
    json->_dontFree = true;
    cJSON_AddItemToObject(_json,pProperty.c_str(),json->GetJsonObject());
    return json;
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

void Json::GetAsArrayElements(std::vector<Json*>& pElements) {
    auto size = cJSON_GetArraySize(_json);
    for(auto i = 0; i < size; i++)
        pElements.push_back(new Json(cJSON_GetArrayItem(_json,i)));
}
