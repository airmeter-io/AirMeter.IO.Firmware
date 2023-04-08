#pragma once

#include<string>
#include<vector>



class StringValueSource {
public:
    virtual std::vector<int> ResolveTimeSeries(std::string pName, uint32_t pSecondsInPast, uint32_t pSteps) = 0;
};

typedef struct {
    bool isValue;
    std::string content;

} StringValueItem;

class StringWithValues {
    std::vector<StringValueItem> _parts;
    std::string ResolveValue(std::string pName);
public:
    StringWithValues();
    StringWithValues(const std::string& pString);

    void Generate(std::string& pOutput);
};