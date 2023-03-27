// #include "Wifi.h"
// #include "CaptiveDNS.h"

// #include "SsdDisplay.h"
// #include "WifiManager.h"
// #define TAG "WIFI"

// static void dnstask(void *arg)
// {
//     ((WifiPrivate *)arg)->RunCaptiveDNS();
// 	vTaskDelete(NULL);
// }


// Wifi::Wifi(const std::string pSoftApName, const std::string pApPassword) : _softApName(pSoftApName),_apPassword(pApPassword) {
// }

// Wifi::~Wifi() {

// }

// bool Wifi::IsProvisioned() {
//     return _wifiManager.HasStationConfiguration();
// }

// void Wifi::RunCaptiveDNS() {
//     printf("Captive DNS running");
//     CaptiveDns dns;
    
//     while(_isProvisioning) {
      
//        dns.ProcessRequest();
//        esp_task_wdt_reset();
//        vTaskDelay(100 / portTICK_PERIOD_MS);
  
//     }

//     printf("Finished waiting for provisioning\n");
    
    
//     printf("Done provisioning\n");
//     _wifiManager.EnableStationOnly();
// }

// void Wifi::StartProvisioning() {
   
//     _wifiManager.EnableAP(_softApName, _apPassword);
//     _wifiManager.Scan();
    
    
   
//     _isProvisioning = true;
//     printf("Starting captive DNS task\n");
//     xTaskCreate(dnstask, "captivedns", 4096, this, 10, NULL);
// }

// void Wifi::Start() {
//     printf("Starting wifi in station mode\n");
//   //  _wifiManager.EnableStationOnly();
//     _wifiManager.ConnectStation();
// }

// WifiManager& Wifi::GetManager() {
//     return _wifiManager;
// }


// bool Wifi::TestStationCredentialsWPA(const wifi_ap_record_t * pAp, const std::string& pPassword, uint32_t pTimeout) {
//     _wifiManager.SetMemoryMode();
//     _wifiManager.PushStaConfig();
//     auto wasConnected = _wifiManager.IsStationConnected();

//     if(wasConnected) {
//         if(_wifiManager.DisconnectStation()) {
//             while(_wifiManager.IsStationConnected()) {
//                 esp_task_wdt_reset();
//                 taskYIELD();
//             }
//         }
//     }

//     auto result = _wifiManager.ConfigureStationAP(pAp, pPassword);
//     auto start = xTaskGetTickCount();

//     _wifiManager.ConnectStation();

//     if(result) {
//         while(xTaskGetTickCount()  - start< pTimeout && !_wifiManager.IsStationConnected()) {
//             esp_task_wdt_reset();
//             taskYIELD();
//         }

//         result = _wifiManager.IsStationConnected();

//         if(result && _wifiManager.DisconnectStation()) {
//             while(_wifiManager.IsStationConnected()) {
//                 esp_task_wdt_reset();
//                 taskYIELD();
//             }
//         }
//     }

//     _wifiManager.PopStaConfig();
//     if(wasConnected) {
//         _wifiManager.ConnectStation();
//     }

//     _wifiManager.SetFlashMode();

//     return result;
// }

// bool Wifi::SetStationCredentialsWPA(const wifi_ap_record_t * pAp, const std::string& pPassword) {
//     _wifiManager.SetFlashMode();
//     _wifiManager.EnableStationOnly();
//     auto result = _wifiManager.ConfigureStationAP(pAp, pPassword);
//     if(_wifiManager.IsStationConnected())
//         if(_wifiManager.DisconnectStation()) {
//             while(_wifiManager.IsStationConnected()) {
//                 esp_task_wdt_reset();
//                 taskYIELD();
//             }
//         }
//     _wifiManager.ConnectStation();
    
//     _isProvisioning = false;
//     return result;
// }

// wifi_ap_record_t *Wifi::FindNetwork(const std::string& pSsid) {
//     _wifiManager.Scan();
//     auto aps = _wifiManager.GetAvailableAps();
//     auto apCount = _wifiManager.GetAvailableApCount();

//     for(auto i = 0; i<apCount;i++) {
//         auto ap =  &aps[i];
//         if(pSsid.compare((char*)ap->ssid)==0) {
//             return ap;
//         }
//     }
//     return nullptr;
// }

// bool Wifi::IsConnected() {
//     return _wifiManager.IsStationConnected();
// }



