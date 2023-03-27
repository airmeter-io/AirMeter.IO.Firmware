#include "WifiTask.h"
#include "CaptiveDNS.h"

static void WifiStationTaskEntry(void *arg)
{
    ((WifiManagerPrivate *)arg)->StationConnectLoop();
	vTaskDelete(NULL);
}

static void WifiApTaskEntry(void *arg)
{
    ((WifiManagerPrivate *)arg)->ApTaskMain();
	vTaskDelete(NULL);
}

WifiConfiguredNetwork* WifiTask::GetConfiguredNetwork(const WifiAvailableNetwork* pAvailableNetwork) {
    for(auto configuredNetwork : _wifiSettings.GetNetworks()) 
        if(configuredNetwork.second->ssid == pAvailableNetwork->ssid &&
           configuredNetwork.second->authMode == pAvailableNetwork->authmode)
            return configuredNetwork.second;
    return nullptr;
}

WifiSettings WifiTask::_wifiSettings;

bool WifiTask::CompareAvailableNetworks(WifiAvailableNetwork* pNetwork1, WifiAvailableNetwork* pNetwork2)
{
    auto network1 = _wifiSettings.GetNetworks().contains(pNetwork1->ssid) ? _wifiSettings.GetNetworks()[pNetwork1->ssid] : nullptr;
    auto network2 = _wifiSettings.GetNetworks().contains(pNetwork2->ssid) ? _wifiSettings.GetNetworks()[pNetwork2->ssid] : nullptr;
    if(network1 == nullptr && network2!=nullptr) return false;
    if(network1 != nullptr && network2==nullptr) return true;
    if(network1 != nullptr && network2!=nullptr && network1->ssid!=network2->ssid) return network1->priority < network2->priority;
    return (pNetwork1->rssi > pNetwork2->rssi);
}

void WifiTask::ApTaskMain() {
    CaptiveDns dns;
    ApDescription ap;
    ap.gatewayIP = _apIPv4Gateway;
    ap.netmask = _apIPv4Mask;
    ap.startIP = _apIPv4StartIP;
    ap.ssid = _apSSID;
    ap.password = _apPassword;
    _manager->EnableAP(ap);
    while(!HasConfiguredNetworks()) {      
       dns.ProcessRequest();
       esp_task_wdt_reset();
       vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    _manager->DisableAP();
    _apTaskRunning = true;
}

WifiTask::WifiTask(const std::string& pDeviceName, const std::string& pApPassword) : _apSSID(pDeviceName), _apPassword(pApPassword) {
    _manager = new WifiManager(*this);
    _stationSemaphore = xSemaphoreCreateBinary();

}
WifiTask::~WifiTask() {

}

void WifiTask::Init() {
    xTaskCreate(WifiStationTaskEntry, "wifista", 2048, this, 10, NULL);
}


void WifiTask::CreateAPTaskIfNotRunning() {
    if(_apTaskRunning) return;
    _apTaskRunning = true;
    xTaskCreate(WifiApTaskEntry, "wifiap", 2048, this, 10, NULL);
}

#define STARTING_SLEEP_INTERVAL 250
#define CONNECTING_SLEEP_INTERVAL 2500
#define CONNECTED_SLEEP_INTERVAL 7500

void WifiTask::StationConnectLoop() {
    WifiAvailableNetworks foundNetworks;
    WifiAvailableNetworks matchingNetworks(false);
    
    auto connectIndex = 0;
    while(true) {
        if(_wifiSettings.GetNetworks().size() == 0 ) {
            CreateAPTaskIfNotRunning();
        }
        switch(_phase) {
            case WifiTaskConnectPhase::Starting :
                xSemaphoreTake(_stationSemaphore, STARTING_SLEEP_INTERVAL / portTICK_PERIOD_MS);  
                break;
            case WifiTaskConnectPhase::StartConnectSequence :
                if(_wifiSettings.GetNetworks().size() == 0 ) {
                    vTaskDelay(STARTING_SLEEP_INTERVAL / portTICK_PERIOD_MS);  
                    continue;
                }
                _gotIP = false;
                foundNetworks.clearAndFree();    
                matchingNetworks.clear();       
                _manager->Scan();
                SetPhase(WifiTaskConnectPhase::Searching);
                
                break;
            case WifiTaskConnectPhase::Searching :
                xSemaphoreTake(_stationSemaphore, CONNECTING_SLEEP_INTERVAL / portTICK_PERIOD_MS);  
                break;
            case WifiTaskConnectPhase::SearchResults :
                _manager->GetScanResults(foundNetworks);
                std::sort(foundNetworks.begin(), foundNetworks.end(), CompareAvailableNetworks);
                
                
                for(auto network : foundNetworks) {
                    auto configuredNetwork = GetConfiguredNetwork(network);
                    if(configuredNetwork!=nullptr)
                        matchingNetworks.push_back(network);
                }
                connectIndex = 0;
                SetPhase(WifiTaskConnectPhase::TryConnect);
                break;
            
            case WifiTaskConnectPhase::TryConnect :
                if(connectIndex<matchingNetworks.size()) {
                    auto networkToTry = matchingNetworks[connectIndex];
                    auto configuredNetworkToTry = GetConfiguredNetwork(networkToTry);
                    _manager->ConnectStation(networkToTry, configuredNetworkToTry->password);
                    connectIndex++;    
                    SetPhase(WifiTaskConnectPhase::Connecting);
                } else 
                    SetPhase(WifiTaskConnectPhase::Disconnected);
                
                break;
            case WifiTaskConnectPhase::Connecting :
                xSemaphoreTake(_stationSemaphore, CONNECTING_SLEEP_INTERVAL / portTICK_PERIOD_MS); 
                break;
            case WifiTaskConnectPhase::WaitingForIP :
                xSemaphoreTake(_stationSemaphore, CONNECTING_SLEEP_INTERVAL / portTICK_PERIOD_MS); 
                break;
            case WifiTaskConnectPhase::Failed :
                xSemaphoreTake(_stationSemaphore, WifiTaskConnectPhase::TryConnect);
                break;
            case WifiTaskConnectPhase::Connected :
                xSemaphoreTake(_stationSemaphore, CONNECTED_SLEEP_INTERVAL / portTICK_PERIOD_MS);  
                break;
            case WifiTaskConnectPhase::Disconnected :
                time_t elapsedSinceDisconnect = (time(nullptr) - _phaseStart);    
                if(elapsedSinceDisconnect<_wifiSettings.GetWaitOnDisconnectTime()) {          
                    xSemaphoreTake(_stationSemaphore, ((_wifiSettings.GetWaitOnDisconnectTime()- elapsedSinceDisconnect )*1000) / portTICK_PERIOD_MS);  
                } else {
                    SetPhase(WifiTaskConnectPhase::Starting);
                }
                break;
        }
    }
}

void WifiTask::OnWifiStarted() {
     SetPhase(WifiTaskConnectPhase::WaitingForIP);
}

void WifiTask::OnStationConnected(std::string pSSID, uint8_t pChanel, uint8_t pBSSID[6], std::string pAuthMode) {
    SetPhase(WifiTaskConnectPhase::WaitingForIP);
}

void WifiTask::OnStationDisconnected(WifiDisconnectReason pReason) {
    if(_gotIP) {
        _phase = WifiTaskConnectPhase::Disconnected;
    } else {
        _phase = WifiTaskConnectPhase::Failed;
    }
    time(&_phaseStart);
}

void WifiTask::OnStationGotIP(std::string pIp, std::string pNetmask, std::string pGateway) {
    _gotIP = true;
    _staIPv4Addr= pIp;
    _staIPv4Mask = pNetmask;
    _staIPv4Gateway = pGateway;
    SetPhase(WifiTaskConnectPhase::Connected);
    
}

void WifiTask::OnStationLostIP() {
    switch(_phase) {
        case WifiTaskConnectPhase::Disconnected :
        case WifiTaskConnectPhase::Failed :
            break;
        default: 
            SetPhase(WifiTaskConnectPhase::WaitingForIP);            
            break;
    }
}

void WifiTask::OnScanComplete() {
    SetPhase(WifiTaskConnectPhase::SearchResults);
}

void WifiTask::SetPhase(WifiTaskConnectPhase pPhase) {
    _phase = pPhase;
    time(&_phaseStart);
}


bool WifiTask::HasConfiguredNetworks() {
    return _wifiSettings.GetNetworks().size()>0;
}

const std::string& WifiTask::GetValuesSourceName() const {
    return "Wifi";
}



