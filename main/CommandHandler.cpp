#include "CommandHandler.h"
#include "CalibrationCommands.h"
#include "SetupCommands.h"
#include "ViewCommands.h"


CommandHandler::CommandHandler(WifiTask& pWifi, GeneralSettings& pSettings, DataManager& pDataManager, MqttManager& pMqttManager): 
    HttpUrlHandler({"/command"}, HTTP_POST ), _wifi(pWifi),  _settings(pSettings) {
    RegisterCommand(new CalibrationCommand(pSettings));
    RegisterCommand(new SaveSettingsCommand(pSettings));
    RegisterCommand(new LoadSettingsCommand(pSettings));
    RegisterCommand(new GetAvailableWifiNetworksCommand(pSettings,pWifi));
    RegisterCommand(new GetCurrentWifiNetworkCommand(pSettings));
    RegisterCommand(new SelectWifiNetworkCommand(pSettings,pWifi));
    RegisterCommand(new GetLatestDataCommand());
    RegisterCommand(new GetSystemInfoCommand(pSettings));
    RegisterCommand(new GetHistoricalDataCommand(pDataManager));
    RegisterCommand(new DataManagementCommand(pSettings, pDataManager));
    RegisterCommand(new MqttManagementCommand(pMqttManager, pSettings));
}

CommandHandler::~CommandHandler() {

}

void CommandHandler::ProcessRequest(HttpRequest *pReq) {
    ProcessAsJsonRequest(pReq);
}