#include "StringWithValues.h"

StringWithValues::StringWithValues() {

}

StringWithValues::StringWithValues(const std::string& pString) {
    std::string cur = "";
    bool isValue = false;
    for(auto ch : pString) {
        if(isValue) {
            if(ch=='}') {
                isValue = false;
                StringValueItem item = { .isValue = true, .content = cur};
                _parts.push_back(item);
                cur.clear();
            } else
                cur+=ch;
        } else {
            if(ch=='{') {
                isValue = true;
                StringValueItem item = { .isValue = false, .content = cur};
                _parts.push_back(item);
                cur.clear();
            } else
                cur+=ch;
        }
    }

    if(!cur.empty()) {
        if(isValue) {
            StringValueItem item = { .isValue = true, .content = cur};
            _parts.push_back(item);
        } else {
            StringValueItem item = { .isValue = false, .content = cur};
            _parts.push_back(item);
        }
    }
}

void StringWithValues::Generate(StringValueSource& pValueSource, std::string& pOutput) {
    pOutput = "";
    for(auto part : _parts) 
        if(part.isValue)
            pOutput+=pValueSource.ResolveValue(part.content);
        else
            pOutput+=part.content;
}