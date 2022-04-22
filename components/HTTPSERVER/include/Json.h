#pragma once
extern "C" {
    #include <stdio.h>
    #include <assert.h>
    #include "esp_log.h"
    #include "cJSON.h"
}


#include <string>
#include <exception>
#include <vector>



class JsonException : public std::exception {
public:
    JsonException() {}
};

class Json ;
#include "JsonArray.h"




class Json {
    cJSON *_json ;
    bool _dontFree = false;
    Json(cJSON* pJson);
public:
    Json();
    Json(std::string pJson);
    std::string GetStringProperty(const std::string& pProperty);
    bool GetBoolProperty(const std::string& pProperty);
    int GetIntProperty(const std::string& pProperty);
    uint GetUIntProperty(const std::string& pProperty);
    Json* GetObjectProperty(const std::string & pProperty);
    bool HasProperty(const std::string& pProperty);
    bool HasObjectProperty(const std::string& pProperty);
    bool HasArrayProperty(const std::string& pProperty);
    void GetAsArrayElements(std::vector<Json*>& pElements);
    void CreateStringProperty(const std::string& pProperty, const std::string& pValue);
    void CreateNumberProperty(const std::string& pProperty, uint pValue);
    void CreateNumberProperty(const std::string& pProperty, int pValue);
    void CreateNumberProperty(const std::string& pProperty, double pValue);
    void CreateBoolProperty(const std::string& pProperty, bool pValue);
    void CreateArrayProperty(const std::string& pProperty, std::vector<Json*>& pElements);
    std::string Print();
    cJSON* GetJsonObject();
    ~Json();
};