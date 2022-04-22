#include "FontManager.h"
#include "Json.h"
#include <string>
#include <stdio.h>
#include <vector>

FontHolder::FontHolder(const std::string &pName,const std::string& pPath) : _font(new BitmapFont(pPath)), _name(pName) {
}

BitmapFont *FontHolder::GetFont() {
    return _font;
}

std::string& FontHolder::GetName() {
    return _name;
}


FontManager::FontManager() {
    auto f = fopen("/dev/fonts.json", "rb");
    if (f != NULL) {
        
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET); 

        auto* rawJson = (char *)calloc(fsize + 1,1);
        fread(rawJson, 1, fsize, f);
        fclose(f);

        Json json(rawJson);
        LoadFonts(json);
        free(rawJson);
    }
}

FontManager::~FontManager() {
    for(auto font : _fonts)
        delete font;
}

void FontManager::LoadFonts(Json& pJson) {
    std::vector<Json*> elements;
    pJson.GetAsArrayElements(elements);
    for(auto json : elements) {
        if(json->HasProperty("Name") && json->HasProperty("File")) {
            auto name = json->GetStringProperty("Name");
            auto file =std::string("/dev/")+ json->GetStringProperty("File");

            _fonts.push_back(new FontHolder(name,file));
        }
        delete json;        
    }
}

FontHolder* FontManager::GetFont(const std::string& pName) {
    for(auto font : _fonts) {
        if(font->GetName() == pName)
            return font;
    }
    return nullptr;
}

FontHolder *FontManager::GetDefaultFont() {
    return _fonts.front();
}

