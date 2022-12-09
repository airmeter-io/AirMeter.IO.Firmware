#include "Utility.h"

std::string GetCurrentIsoTimeString() {
    
    time_t now;
    time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    return buf;
}

void ReplaceAll( std::string &pString, const std::string &pSearch, const std::string &pReplace) {
    for(size_t pos = 0; ; pos += pReplace.length()) {
        pos = pString.find(pSearch, pos);
        if(pos == std::string::npos) break;
        pString.erase(pos, pSearch.length());
        pString.insert(pos, pReplace);
    }
}

inline uint8_t HexDigitToNibble(char pDigit, bool& pFailed) {
    if((uint8_t)pDigit>=(uint8_t)'a' && (uint8_t)pDigit<=(uint8_t)'f')
        return ((uint8_t)pDigit - (uint8_t)'a')+10;
    if((uint8_t)pDigit>=(uint8_t)'A' && (uint8_t)pDigit<=(uint8_t)'F')
        return ((uint8_t)pDigit - (uint8_t)'A')+10;        
    if((uint8_t)pDigit>=(uint8_t)'0' && (uint8_t)pDigit<=(uint8_t)'9')
        return ((uint8_t)pDigit - (uint8_t)'0');
    pFailed = true;
    return 0;
}

bool HexStringToBytes(const std::string &pString, uint8_t **pNewBuffer, uint16_t *pLength) {
    if(pString.length()%2) return false;
    auto byteLength = pString.length()/2;
    auto data = (uint8_t *)malloc(byteLength);
    if(data==nullptr) return false;

    auto dataHexCstr = pString.c_str();
    for(auto i = 0, offset = 0; i < byteLength;i++, offset+=2) {
        bool failed = false;
        auto high = HexDigitToNibble(dataHexCstr[offset], failed);
        auto low = HexDigitToNibble(dataHexCstr[offset+1], failed);
        if(failed) {
            free(data);
            return false;
        } 
        data[i] = (high<<4) | low;
    }
    *pNewBuffer = data;
    *pLength = byteLength;
    return true;
}