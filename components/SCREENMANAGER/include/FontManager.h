#pragma once 

#include "BitmapFont.h"
#include "Json.h"
#include <vector>
#include <string>
class FontHolder {
    BitmapFont* _font;
    std::string _name;
public:
    FontHolder(const std::string &pName, const std::string& pPath);

    BitmapFont *GetFont();
    std::string& GetName();
};

class FontManager {
    std::vector<FontHolder*> _fonts;
    void LoadFonts(Json& pJson);

public:
    FontManager();
    ~FontManager();

    FontHolder* GetFont(const std::string& pName);
    FontHolder *GetDefaultFont();
};