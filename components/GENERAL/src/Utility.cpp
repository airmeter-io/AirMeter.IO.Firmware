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