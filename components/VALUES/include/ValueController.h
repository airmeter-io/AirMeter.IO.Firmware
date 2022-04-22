#pragma once
#include "Common.h"
#include<string>
#include<vector>
#include<map>


enum ValueType { Bool, Int, Fixed, Double, String};
enum ValueUnit { None, Centigrade, Percent, hPa, PPM };
class Value {
public:
    ValueType type;
    ValueUnit unit;
    union {
        bool b;
        int i;
        struct {
          int n;
          uint16_t d;
        } f;
        double d;
        std::string* s;
    } value;
    std::string name;
    int priority;
};


class ValueSource {
    std::string& _name;
public:
    ValueSource(const std::string& pName);

    std::string& GetName() const;
}


class ValueContainer {
    std::map<ValueSource*, Value*> _values
public:

}

class ValueController {  
    std::vector<ValueSource*> _sources;

public:
    ValueController();
    ~ValueController();
    void RegisterSource(ValueSource* pSource);
    void RegisterValue(ValueSource *pValueSource, Value  *pValue)
};