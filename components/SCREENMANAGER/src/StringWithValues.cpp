#include "StringWithValues.h"
#include "ValueController.h"

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

std::string StringWithValues::ResolveValue(std::string pName) {
    auto  pos = pName.find (".");
    if(pos!=std::string::npos) {
        auto group =  pName.substr(0, pos);
        auto name = pName.substr(pos+1,std::string::npos);
        auto source = ValueController::GetCurrent().GetDefault(group, name);
        if(source) return source->GetValueAsString();
    }
   
    
    return "ERR";
 }

void StringWithValues::Generate(std::string& pOutput) {
    pOutput = "";
    for(auto part : _parts) 
        if(part.isValue)
            pOutput+=ResolveValue(part.content);
        else
            pOutput+=part.content;
}