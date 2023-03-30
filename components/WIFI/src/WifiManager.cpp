#include "WifiManager.h"

#define TAG "WifiManager"


extern "C" {
    #include "esp_mac.h"
}

static void WifiManageEventHandler(void* pArg, esp_event_base_t pEventBase, int32_t pEventId, void* pEventData)
{
    if (pEventBase == WIFI_EVENT) 
         ((WifiManagerInternal*)pArg)->ProcessWifiEvent((wifi_event_t)pEventId, pEventData);
    else if (pEventBase == IP_EVENT) 
        ((WifiManagerInternal*)pArg)->ProcessIPEvent((ip_event_t)pEventId, pEventData);
}

WifiAvailableNetworks::WifiAvailableNetworks(bool pFreeOnDispose) : _freeOnDispose(pFreeOnDispose) {

}

WifiAvailableNetworks::~WifiAvailableNetworks() {
    if(_freeOnDispose)
        for(auto network : *this) 
            delete network;
}

void WifiAvailableNetworks::clearAndFree() {
    for(auto network : *this) 
        delete network;
    
    clear();
}


void WifiManager::ProcessWifiEvent(wifi_event_t pEventId, void *pEvent) {
    printf("Processing wifi event %d\n", (int)pEventId);
    switch(pEventId) {
        case WIFI_EVENT_WIFI_READY :
            ProcessWifiReadyEvent();
            break;
        case WIFI_EVENT_SCAN_DONE :
            ProcessScanDoneEvent((wifi_event_sta_scan_done_t*)pEvent);
            break;
        case WIFI_EVENT_STA_START :
            ProcessWifiStationStartEvent();
            break;
        case WIFI_EVENT_STA_STOP :
            ProcessWifiStationStopEvent();
            break;
        case WIFI_EVENT_STA_CONNECTED :
            ProcessWifiStationConnectedEvent((wifi_event_sta_connected_t*)pEvent);
            break;
        case WIFI_EVENT_STA_DISCONNECTED :
            ProcessWifiStationDisconnectedEvent();
            break;
        case WIFI_EVENT_STA_AUTHMODE_CHANGE :
            ProcessWifiStationAuthmodeChangeEvent((wifi_event_sta_authmode_change_t *)pEvent);
            break;
        case WIFI_EVENT_STA_BSS_RSSI_LOW :
            ProcessWifiStationSignalToLow();
            break;
        case WIFI_EVENT_STA_WPS_ER_SUCCESS :
            break;
        case WIFI_EVENT_STA_WPS_ER_FAILED :
            break;
        case WIFI_EVENT_STA_WPS_ER_TIMEOUT :
            break;
        case WIFI_EVENT_STA_WPS_ER_PIN :
            break;
        case WIFI_EVENT_AP_START :
            printf("Ap start\n");
            ProcessAccessPointStartEvent();
            break;
        case WIFI_EVENT_AP_STOP :
            printf("Ap stop\n");
            ProcessAccessPointStopEvent();
            break;
        case WIFI_EVENT_AP_STACONNECTED :
            printf("Ap sta connected\n");
            ProcessWifiAccessPointStationConnectedEvent((wifi_event_ap_staconnected_t*)pEvent);
            break;
        case WIFI_EVENT_AP_STADISCONNECTED :
            printf("Ap sta disnnected\n");
            ProcessWifiAccessPointStationDisconnectedEvent((wifi_event_ap_stadisconnected_t*)pEvent);
            break;
        case WIFI_EVENT_AP_PROBEREQRECVED :
            printf("Ap probe\n");
            ProcessWifiAccessPointProbeReqRecievedEvent((wifi_event_ap_probe_req_rx_t*)pEvent);
            break;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
        case WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP :      
            break;
        case WIFI_EVENT_FTM_REPORT :      
            break;
        case WIFI_EVENT_ACTION_TX_STATUS :      
            break;
        case WIFI_EVENT_ROC_DONE :      
            break;
        case WIFI_EVENT_STA_BEACON_TIMEOUT :      
            break;

        case WIFI_EVENT_MAX :      
            break;         
                       
#endif  
        default:
            break;

    }
}

void WifiManager::ProcessIPEvent(ip_event_t pEvent, void *pEventData) {
    switch(pEvent) {
        case IP_EVENT_STA_GOT_IP :
            ProcessGotIPEvent((ip_event_got_ip_t *)pEventData);
            break;
        case IP_EVENT_STA_LOST_IP :
            ProcessLostIPEvent();
            break;
        case IP_EVENT_AP_STAIPASSIGNED :
            ProcessGotStaAssignedIPEvent((ip_event_ap_staipassigned_t *)pEventData);
            break;
        case IP_EVENT_GOT_IP6 :
            ProcessGotIPV6Event((ip_event_got_ip6_t *)pEventData);
            break;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
        case IP_EVENT_ETH_GOT_IP :      
            break;
        case IP_EVENT_PPP_GOT_IP :      
            break;
        case IP_EVENT_PPP_LOST_IP :      
            break;  
        case IP_EVENT_ETH_LOST_IP : 
            break;                                
#endif 
            
    }
}

void WifiManager::ProcessGotIPEvent(ip_event_got_ip_t* pEvent) {
    char ipBuf[32];
    printf("ProcessGotIPEvent\n");
    snprintf(ipBuf, sizeof(ipBuf), IPSTR, IP2STR(&pEvent->ip_info.ip));

    
    char nmBuf[32];
    snprintf(nmBuf, sizeof(nmBuf), IPSTR, IP2STR(&pEvent->ip_info.netmask));


    char gwBuf[32];
    snprintf(gwBuf, sizeof(gwBuf), IPSTR, IP2STR(&pEvent->ip_info.gw));
    
    _callbacks.OnStationGotIP(ipBuf, nmBuf, gwBuf);
    printf("ProcessGotIPEvent-Exit\n");
}

void WifiManager::ProcessLostIPEvent() {
    ESP_LOGI(TAG, "Lost IP");
}

void WifiManager::ProcessGotIPV6Event(ip_event_got_ip6_t* pEvent) {
}

void WifiManager::ProcessGotStaAssignedIPEvent(ip_event_ap_staipassigned_t* pEvent) {
    char ipBuf[32];
    if(pEvent==nullptr) return;
    snprintf(ipBuf, sizeof(ipBuf), IPSTR, IP2STR((ip4_addr_t*)pEvent));
    std::string ip = ipBuf;
    std::string msg = "A Station was assigned IP: "+ip;
    ESP_LOGI(TAG,"%s", (msg.c_str()));

}

void WifiManager::ProcessWifiReadyEvent() {
    ESP_LOGI(TAG, "Wifi Ready");
}

void WifiManager::ProcessScanDoneEvent(wifi_event_sta_scan_done_t* pEvent) {
    ESP_LOGI(TAG, "Scan Done: %d, Succeeded: %s", pEvent->number, !pEvent->status ? "Yes" : "No");
    if(!pEvent->status)
        _callbacks.OnScanComplete();
    else 
        ESP_LOGI(TAG, "Scan Done: %d, FAILED", pEvent->number);

}

void WifiManager::ProcessWifiStationStartEvent(){
    _callbacks.OnWifiStarted();
}

void WifiManager::ProcessWifiStationStopEvent(){
    ESP_LOGI(TAG, "Wifi Station Stopped");
}

void WifiManager::ProcessWifiStationConnectedEvent(wifi_event_sta_connected_t* pEvent){
    printf("Wifi Station Connected to: %s", (char*)pEvent->ssid);
    _callbacks.OnStationConnected(
        std::string((char*)pEvent->ssid, pEvent->ssid_len),
        pEvent->channel,
        pEvent->bssid,
        GetAuthModeText(pEvent->authmode));
}

void WifiManager::ProcessWifiStationDisconnectedEvent() {
    ESP_LOGI(TAG, "Wifi Station Disconnected");
    _isStaConnected = false;
}

void WifiManager::ProcessWifiStationAuthmodeChangeEvent(wifi_event_sta_authmode_change_t *pEvent){
    ESP_LOGI(TAG, "Wifi Station Authmode changed from %s to %s", GetAuthModeText(pEvent->old_mode).c_str(), GetAuthModeText(pEvent->new_mode).c_str());
}

void WifiManager::ProcessWifiStationSignalToLow() {
    ESP_LOGI(TAG, "Wifi Station Signal To Low");
    _isStaConnected = false;
}

void WifiManager::ProcessAccessPointStartEvent(){
  ESP_LOGI(TAG, "Wifi AP Started"); 
}


void WifiManager::ProcessAccessPointStopEvent(){
    ESP_LOGI(TAG, "Wifi AP Stopped");
}

void WifiManager::ProcessWifiAccessPointStationConnectedEvent(wifi_event_ap_staconnected_t* pEvent) {
    printf("Wifi AP Station Connected %.2X:%.2X:%.2X:%.2X:%.2X:%.2X (AID: %d)", 
        (int)pEvent->mac[0], (int)pEvent->mac[1], (int)pEvent->mac[2], (int)pEvent->mac[3], (int)pEvent->mac[4], (int)pEvent->mac[5],
        (int)pEvent->aid);
}

void WifiManager::ProcessWifiAccessPointStationDisconnectedEvent(wifi_event_ap_stadisconnected_t* pEvent){
    ESP_LOGI(TAG, "Wifi AP Station Disconnected %.2X:%.2X:%.2X:%.2X:%.2X:%.2X (AID: %d)", 
        (int)pEvent->mac[0], (int)pEvent->mac[1], (int)pEvent->mac[2], (int)pEvent->mac[3], (int)pEvent->mac[4], (int)pEvent->mac[5],
        (int)pEvent->aid);
}

void WifiManager::ProcessWifiAccessPointProbeReqRecievedEvent(wifi_event_ap_probe_req_rx_t* pEvent) {
    ESP_LOGI(TAG, "Wifi AP Probe Received From %.2X:%.2X:%.2X:%.2X:%.2X:%.2X (RSSI: %d)", 
        (int)pEvent->mac[0], (int)pEvent->mac[1], (int)pEvent->mac[2], (int)pEvent->mac[3], (int)pEvent->mac[4], (int)pEvent->mac[5],
        (int)pEvent->rssi);
}

typedef struct {
    wifi_auth_mode_t mode;
    std::string name;
} AuthModeMapping;



const AuthModeMapping AuthModeMappings[11] = {
    {.mode = WIFI_AUTH_OPEN,
     .name = "WIFI_AUTH_OPEN"},
    {.mode = WIFI_AUTH_WEP,
     .name = "WIFI_AUTH_WEP"},     
    {.mode = WIFI_AUTH_WPA_PSK,
     .name = "WIFI_AUTH_WPA_PSK"},   
    {.mode = WIFI_AUTH_WPA2_PSK,
     .name = "WIFI_AUTH_WPA2_PSK"},   
    {.mode = WIFI_AUTH_WPA_WPA2_PSK,
     .name = "WIFI_AUTH_WPA_WPA2_PSK"},   
    {.mode = WIFI_AUTH_WPA2_ENTERPRISE,
     .name = "WIFI_AUTH_WPA2_ENTERPRISE"},   
    {.mode = WIFI_AUTH_WPA3_PSK,
     .name = "WIFI_AUTH_WPA3_PSK"},   
    {.mode = WIFI_AUTH_WPA2_WPA3_PSK,
     .name = "WIFI_AUTH_WPA2_WPA3_PSK"},   
    {.mode = WIFI_AUTH_WAPI_PSK,
     .name = "WIFI_AUTH_WAPI_PSK"},   
    {.mode = WIFI_AUTH_WAPI_PSK,
     .name = "WIFI_AUTH_WAPI_PSK"},   
    {.mode = WIFI_AUTH_OWE,
     .name = "WIFI_AUTH_OWE"}                                               
};

std::string WifiManager::GetAuthModeText(wifi_auth_mode_t pAuthMode) {
    if(pAuthMode>= wifi_auth_mode_t::WIFI_AUTH_MAX)
        return "UNKNOWN";
    return AuthModeMappings[((int)pAuthMode)].name;
}

wifi_auth_mode_t WifiManager::GetAuthModeFromText(std::string pAuthMode) {
    for(auto mode : AuthModeMappings)
        if(mode.name == pAuthMode)
            return mode.mode;
    return wifi_auth_mode_t::WIFI_AUTH_MAX; 
}

WifiManager::WifiManager(WifiManagerCallBacks& pCallbacks) : _callbacks(pCallbacks) {
    uint8_t mac[6];


    ESP_ERROR_CHECK(esp_efuse_mac_get_default(mac));
    ESP_ERROR_CHECK(esp_base_mac_addr_set(mac));
    esp_netif_init();
    _staIf  = esp_netif_create_default_wifi_sta();
    //ESP_ERROR_CHECK(esp_event_loop_create_default());
    _initConfig  = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&_initConfig);
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiManageEventHandler, this));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &WifiManageEventHandler, this));
}

WifiManager::~WifiManager() {
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiManageEventHandler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, &WifiManageEventHandler));
}

bool WifiManager::HasStationConfiguration() {
    wifi_config_t config;
    if(esp_wifi_get_config((wifi_interface_t)ESP_IF_WIFI_STA, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Cannot get existing configuration");
        return false;
    }

    if(strlen((const char *) config.sta.ssid)) {
        printf("SSID=%s, PW=%s\n", config.sta.ssid, config.sta.password);
        return true;
    }
    return false;
}


bool WifiManager::EnableAP(const ApDescription& pAp, uint8_t pMaxConnections) {
    printf("Enabling AP\n");
    if(_isApStarted) return false;
    
    _apIf = esp_netif_create_default_wifi_ap();
    esp_netif_dhcps_stop(_apIf);  

    esp_netif_ip_info_t ip_info;
    inet_aton(pAp.startIP.c_str(),&ip_info.ip);
    inet_aton(pAp.gatewayIP.c_str(),&ip_info.gw);
    inet_aton(pAp.netmask.c_str(), &ip_info.netmask);    
    ESP_ERROR_CHECK(esp_netif_set_ip_info(_apIf, &ip_info));
    ESP_ERROR_CHECK(esp_netif_dhcps_start(_apIf));

    wifi_config_t config;
    memset(&config, 0, sizeof(config));

    config.ap.max_connection = pMaxConnections;
    strlcpy((char *)config.ap.ssid,pAp.ssid.c_str(), sizeof(config.ap.ssid));
    config.ap.ssid_len = pAp.ssid.length();
    if(pAp.password.length()>0) 
    {
        strlcpy((char *)config.ap.password, pAp.password.c_str(), sizeof(config.ap.password));
        printf("Set AP PW = '%s'", (char*)config.ap.password);
        config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    } else 
        config.ap.authmode = WIFI_AUTH_OPEN;


    
    auto err = esp_wifi_set_mode(WIFI_MODE_APSTA);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable AP mode: %d", err);
        return false;
    }
 
    err = esp_wifi_set_config((wifi_interface_t)ESP_IF_WIFI_AP, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi config : %d", err);
        return false;
    }
    
   
    ESP_ERROR_CHECK(esp_wifi_start());
    _wifiIsStarted = true;
    return true;
}


bool WifiManager::DisableAP() {
    printf("Disabling AP\n");
    auto err = esp_wifi_set_mode(WIFI_MODE_STA);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable STA mode: %d", err);
        return false;
    }
    return true;

}


bool WifiManager::DisableWifi(){
    auto err = esp_wifi_set_mode(WIFI_MODE_NULL);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set NULL mode: %d", err);
        return false;
    }
    return true;
}


void WifiManager::Scan() {
    wifi_scan_config_t scanCfg;
    memset(&scanCfg, 0, sizeof(scanCfg));
    scanCfg.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    scanCfg.scan_time.active.min = 120;
    scanCfg.scan_time.active.max = 120;
    scanCfg.channel = 0;
    if (esp_wifi_scan_start(&scanCfg, false) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start scan");
        return;
    }
}

void WifiManager::GetScanResults(WifiAvailableNetworks& pAvailableNetworks) {    

    
    uint16_t numRecords;
    if (esp_wifi_scan_get_ap_num(&numRecords)!= ESP_OK) {
        ESP_LOGE(TAG, "Failed to get number of records");
        return;
    }


    auto availableAps = (wifi_ap_record_t*)calloc(sizeof(wifi_ap_record_t),numRecords);
    if(availableAps==NULL){        
        ESP_LOGE(TAG, "Failed to allocate memory for Wifi records");
        return;
    }

    if (esp_wifi_scan_get_ap_records(&numRecords, availableAps)!= ESP_OK) {
        
        free(availableAps);
        ESP_LOGE(TAG, "Failed to get Wifi records");
        return;
    }

    for(auto i = 0; i < numRecords; i++){
        WifiAvailableNetwork* network = new WifiAvailableNetwork();
        memcpy(network->bssid, availableAps[i].bssid, sizeof(network->bssid));
        network->ssid = (char *)availableAps[i].ssid;
        network->channel = availableAps[i].primary;
        network->rssi = availableAps[i].rssi;
        network->authmode = GetAuthModeText(availableAps[i].authmode);
        pAvailableNetworks.push_back(network);
    }

    free(availableAps);
}

bool WifiManager::ConnectStation(const WifiAvailableNetwork* pNetwork, const std::string& pPassword) {
    if(!_wifiIsStarted) {
        esp_wifi_start();
        _wifiIsStarted = true;
    }

    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    memset(&_wifiConfig,0, sizeof(_wifiConfig));

    memset(_wifiConfig.sta.bssid,0, sizeof(_wifiConfig.sta.bssid));
    memcpy(_wifiConfig.sta.bssid, pNetwork->bssid, sizeof(_wifiConfig.sta.bssid));
    _wifiConfig.sta.bssid_set = true;
    _wifiConfig.sta.btm_enabled = false;
    _wifiConfig.sta.channel = pNetwork->channel;
    _wifiConfig.sta.listen_interval  = 0; //6;
    memset(_wifiConfig.sta.password,0, sizeof(_wifiConfig.sta.password));
    strncpy((char *)_wifiConfig.sta.password, pPassword.c_str(), sizeof(_wifiConfig.sta.password));
    _wifiConfig.sta.pmf_cfg.capable = false;
    _wifiConfig.sta.pmf_cfg.required = false;
    _wifiConfig.sta.rm_enabled = true;
  //  _wifiConfig.sta.scan_method = WIFI_FAST_SCAN;
 //   _wifiConfig.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
    memset(_wifiConfig.sta.ssid,0, sizeof(_wifiConfig.sta.ssid));
    strncpy((char *)_wifiConfig.sta.ssid, (char *)pNetwork->ssid.c_str(), sizeof(_wifiConfig.sta.ssid));
    _wifiConfig.sta.threshold.rssi = 0;
    _wifiConfig.sta.threshold.authmode = GetAuthModeFromText(pNetwork->authmode); 


    auto err = esp_wifi_set_config((wifi_interface_t)ESP_IF_WIFI_STA, &_wifiConfig);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable STA wifi config: %d", err);
        return false;
    }

   // esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
    ESP_ERROR_CHECK(esp_wifi_connect());
    printf("Connecting...\n");

    return true;
}

bool WifiManager::DisconnectStation() {
    printf("Disconnecting...\n");
    ESP_ERROR_CHECK(esp_wifi_disconnect());

    return true;
}