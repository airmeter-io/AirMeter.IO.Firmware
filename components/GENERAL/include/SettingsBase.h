#pragma once

#include "Json.h"

class SettingsBase {
protected:
    virtual const char* GetFilePath() = 0;
    virtual void LoadSettingsFromJson(Json& pJson);
    virtual void SaveSettingsToJson(Json& pJson);
public:
    void Load();
    void Save();
};