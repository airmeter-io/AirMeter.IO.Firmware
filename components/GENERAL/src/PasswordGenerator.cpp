#include "PasswordGenerator.h"

PasswordGenerator::PasswordGenerator() {

}

const char *PWCHARS = "0123456789ABCDEFGHIJKLMNOPQRTUVWXYZabcdefghijklmnopqrtsuvwxyz%$!";
std::string PasswordGenerator::Generate(int pLength) {
    std::string result = "";
    auto mod = strlen(PWCHARS);

    for(auto i = 0; i< pLength; i++) {
        int random = (int)(esp_random() % mod);
        while(random<0 || random>=mod)
            random = (int)(esp_random() % mod);
        
        result+=PWCHARS[random];

    }
    return result;
}