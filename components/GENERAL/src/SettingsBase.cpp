#include "SettingsBase.h"
#include<stdio.h>

void SettingsBase::Load() {
    auto f = fopen(GetFilePath(), "rb");
    if (f != NULL) {
        
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET); 

        auto* rawJson = (char *)calloc(fsize + 1,1);
        fread(rawJson, 1, fsize, f);
        fclose(f);

        Json json(rawJson);
        LoadSettingsFromJson(json);
        free(rawJson);
    } else 
        Save();
}

void SettingsBase::Save() {
    auto f = fopen(GetFilePath(), "w");
    if (f == NULL) {
        ESP_LOGE("SettingsBase", "Failed to open file for writing:  %s", GetFilePath());
        return;
    }
    Json json;
    SaveSettingsToJson(json);
    auto jsonStr = json.Print();
    fprintf(f, "%s", jsonStr.c_str());
    fclose(f);
}