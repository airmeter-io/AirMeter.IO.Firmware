#include "JsonArray.h"

JsonArray::JsonArray() {
    _json = cJSON_CreateArray();
}
JsonArray::JsonArray(cJSON*pJson) : _json(pJson) {

}
void JsonArray::Add(Json& pJson) {
    cJSON_AddItemToArray( _json, pJson.GetJsonObject() );
}

cJSON* JsonArray::GetJsonObject() {
    return _json;
}
