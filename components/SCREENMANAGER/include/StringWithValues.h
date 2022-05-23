#pragma once

#include<string>
#include<vector>



class StringValueSource {
public:
    virtual std::string ResolveValue(std::string pName) = 0;
    virtual std::vector<int> ResolveTimeSeries(std::string pName, uint32_t pSecondsInPast, uint32_t pSteps) = 0;
};

typedef struct {
    bool isValue;
    std::string content;

} StringValueItem;

class StringWithValues {
    std::vector<StringValueItem> _parts;
public:
    StringWithValues();
    StringWithValues(const std::string& pString);

    void Generate(StringValueSource& pValueSource, std::string& pOutput);
};