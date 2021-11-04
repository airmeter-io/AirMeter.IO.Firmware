#pragma once

#include "Json.h"

class JsonArray {
    cJSON *_json ;
public:
    JsonArray() ;
    JsonArray(cJSON*pJson);

    void Add(Json& pJson);
    cJSON* GetJsonObject();
};