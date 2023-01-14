#include "CommandHandler.h"
#include "CalibrationCommands.h"
#include "SetupCommands.h"
#include "ViewCommands.h"


CommandHandler::CommandHandler(Wifi& pWifi, GeneralSettings& pSettings, DataManager& pDataManager): 
    HttpUrlHandler({"/command"}, HTTP_POST ), _wifi(pWifi),  _settings(pSettings) {
    RegisterCommand(new GetCalibrationInfoCommand());
    RegisterCommand(new ManualCalibrationCommand());
    RegisterCommand(new EnableAbcCommand());
    RegisterCommand(new DisableAbcCommand());
    RegisterCommand(new SaveSettingsCommand(pSettings));
    RegisterCommand(new LoadSettingsCommand(pSettings));
    RegisterCommand(new GetAvailableWifiNetworksCommand(pSettings,pWifi));
    RegisterCommand(new GetCurrentWifiNetworkCommand(pSettings,pWifi));
    RegisterCommand(new SelectWifiNetworkCommand(pSettings,pWifi));
    RegisterCommand(new GetLatestDataCommand());
    RegisterCommand(new GetSystemInfoCommand(pSettings));
    RegisterCommand(new GetHistoricalDataCommand(pDataManager));
    RegisterCommand(new DataManagementCommand(pDataManager));
}

CommandHandler::~CommandHandler() {

}

void CommandHandler::ProcessRequest(HttpRequest *pReq) {
    ProcessAsJsonRequest(pReq);
}