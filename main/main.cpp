
#include "main.h"

#include "I2C.h"
#include "OledSsd1306Display.h"
#include "HttpServer.h"
#include "CaptiveRedirectHandler.h"
#include "WebContentHandler.h"
#include "CommandHandler.h"
#include "SntpManager.h"
#include <dirent.h> 
#include "PasswordGenerator.h"
#include "DataManagerQuery.h"
#include "PowerManagement.h"
#include "PowerLock.h"

extern "C" {
    #include "esp_sleep.h" 
}

#define TAG "CO2 Monitor"

MainLogicLoop *mainLoop;

static void i2ctask(void *arg)
{
    mainLoop->Run();
	vTaskDelete(NULL);
}

MainLogicLoop::MainLogicLoop() {
    GpioManager::Setup();
    _i2c = new I2C(GPIO_NUM_4, GPIO_NUM_5);
    _i2c->Scan();
    _devicePersonality = DevicePersonality::Load();
    _generalSettings = new GeneralSettings();
    _ramStore = new DataManagerRamTemporaryStore();
    _flashStore = new DataManagerFlashDataStore();
    _dataManager = new DataManager(_flashStore,_ramStore);
    _sensorManager = new SensorManager(*_devicePersonality, *_generalSettings, *_i2c, *_dataManager);
    _screenDrivers = new ScreenDrivers();


    DrawControl *display;
    auto displayConfig = _devicePersonality->GetDisplayConfig();
    if(displayConfig.Enabled) {
        printf("Loading display driver {%s}\n", displayConfig.Driver.c_str());
        display = _screenDrivers->LoadDriver(displayConfig.Driver, displayConfig.DriverConfig);

    } else 
        display = new NullDrawControl();
    _screenManager = new ScreenManager(*_devicePersonality, display, *this, *_sensorManager, *this);   
    

    AddValueSource(new ValueSource(*this,SYSTEM_UNIXTIME,     Int,    Dimensionless, _valUnixTime,     GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SYSTEM_TIMESTRING,   String, Dimensionless, _valTimeString,   VALUESOURCE_NOFLAGS));
    AddValueSource(new ValueSource(*this,SYSTEM_VERSION,      String, Dimensionless, _valVersion,      VALUESOURCE_NOFLAGS | DEFAULT_MQTTINFO));
    AddValueSource(new ValueSource(*this,SYSTEM_BUILD,        String, Dimensionless, _valBuild,        VALUESOURCE_NOFLAGS | DEFAULT_MQTTINFO));
    AddValueSource(new ValueSource(*this,SYSTEM_BATTERYVOLTS, Fixed,  Dimensionless, _valBatteryVolts, GET_LATEST_DATA  | DEFAULT_MQTT));

    RegisterWithValueController();

}

bool MainLogicLoop::ProcessOnUIThread() {
    bool requiresRedraw = false;

    time_t now;
    time(&now);
    if(now / 60 != _lastTimeInMinutes) {
        _lastTimeInMinutes = now / 60;
        char buf[sizeof "07:07:09Z"];
        strftime(buf, sizeof buf, "%H:%M", gmtime(&now));
        _timeString = buf;
        requiresRedraw = true;
    }

    if(_waitingForProvisioning && _wifi->HasConfiguredNetworks()) {
        _waitingForProvisioning = false;
        _screenManager->ChangeScreen("HOME");
    }

    if(_uiProcessCount%5 == 0) {
        auto voltage = _battery->GetBatteryVoltage();
        auto major = (int)voltage;
        auto minor = (int)((voltage - major)*100);        
        _valBatteryVolts.i = major * 100 + minor;
    }
    _uiProcessCount++;
    _sensorManager->UpdateValues();
    if(_sensorManager->GetLastSensorRead()>_lastRead && _screenManager!=nullptr) {
        printf("Triggering update\n");
        _lastRead = _sensorManager->GetLastSensorRead();
        requiresRedraw = true;
    }


    if(_mqtt!=nullptr) _mqtt->Tick();

    if(!_gotNtp) {
        for(auto i = 0; i< 2; i++) {
            if(sntp_getserver(i)!=0) {
                ESP_LOGI(TAG,"NTP %s\n", ipaddr_ntoa(sntp_getserver(i)));
                _gotNtp = true;
                requiresRedraw = true;
            }
        }
    }

    return requiresRedraw;
}

void MainLogicLoop::Run() {
    
    _battery = new BatteryManager();
    auto httpServer = new HttpServer();
    

    if(_generalSettings->GetApPassword().length()==0) {
        PasswordGenerator pwGen;
        _generalSettings->SetApPassword(pwGen.Generate(24));
        _generalSettings->Save();
    }
    auto apName = _generalSettings->GetDeviceName();
    auto apPasword = _generalSettings->GetApPassword();
    _wifi = new WifiTask(apName,apPasword);
    _wifi->RegisterWithValueController();
    ValueController::GetCurrent().Load();
    _wifi->Init();
    CommandHandler *command = nullptr;
    CaptiveRedirectHandler* captiveRedirect = nullptr;
    WebContentHandler *webContent = nullptr;
    _mqtt = new MqttManager(*_generalSettings);
    httpServer->Start();
 
    command = new CommandHandler(*_wifi, *_generalSettings, *_dataManager, *_mqtt);
    webContent = new WebContentHandler();
    httpServer->AddUrlHandler(webContent);
    httpServer->AddUrlHandler(command);
    SntpManager *sntp = nullptr;

    if(!_wifi->HasConfiguredNetworks()) {
        _screenManager->ChangeScreen("CAPTIVE");
        captiveRedirect = new CaptiveRedirectHandler ();
        httpServer->AddUrlHandler(captiveRedirect); 
        _waitingForProvisioning = true;
    } else {
        sntp = new SntpManager(_generalSettings->GetNtpServers(), _generalSettings->GetEnableDhcpNtp());
        sntp->Init();       
    }

    _sensorManager->UpdateValues();
   
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    esp_sleep_enable_wifi_wakeup();
#endif
   _screenManager->Run(5000 / portTICK_PERIOD_MS);

}

std::vector<int> MainLogicLoop::ResolveTimeSeries(std::string pName, uint32_t pSecondsInPast, uint32_t pSteps) {
    std::vector<int> result(pSteps,0);
    std::vector<int> resultCounts(pSteps,0);
    if(pName!="CO2Ppm" &&
       pName!="Temp" &&
       pName!="Pressure" &&
       pName!="Humidity")
       return std::vector<int>();
    

    time_t curTime;
    time(&curTime);
    if(curTime % (pSecondsInPast/pSteps)) 
        curTime+=(pSecondsInPast/pSteps)-(curTime % (pSecondsInPast/pSteps));

    auto startTime = curTime-pSecondsInPast;
    auto query = _dataManager->StartQuery(startTime, curTime);
    const auto maxEntries = 60;
    const auto multiplier = 100;
    auto entries = (DataEntry*)calloc(maxEntries, sizeof(DataEntry));

    auto read = query->ReadEntries(entries, maxEntries);
    auto perStep = (pSecondsInPast*multiplier)/pSteps;
    while(read>0) {
        for(auto i = 0; i < read; i++)
        {
            auto step = ((entries[i].TimeStamp-startTime)*multiplier)/perStep;

            if(step<0 || step >= pSteps) {
                printf("Step out of range %d\n",(int)step);
                continue;
            }
            if(pName=="CO2Ppm")
                result[step]+=entries[i].CO2;
            else if (pName=="Temp")
                result[step]+=entries[i].Temp;
            else if (pName=="Pressure") 
                result[step]+=entries[i].Pressure;
            else if (pName=="Humidity") 
                result[step]+=entries[i].Humidity;
            
            resultCounts[step]++;
        }
        esp_task_wdt_reset();
        read = query->ReadEntries(entries, maxEntries);
        esp_task_wdt_reset();
    }
    int last = 0;
    for(auto i = 0; i< pSteps; i++) {
        if(resultCounts[i])
        {
            result[i]=result[i]/resultCounts[i];
            last = result[i];
        } else
            result[i] = last;
    }
    free(entries);
    delete query;
    return result;
}

const std::string SystemSourceName = "System";
const std::string& MainLogicLoop::GetValuesSourceName() const {
    return SystemSourceName;
}


bool MountSpiffs(esp_vfs_spiffs_conf_t& pConf) {
    esp_err_t ret = esp_vfs_spiffs_register(&pConf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return false;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "%s: total: %d, used: %d",pConf.base_path, total, used);
    }

    DIR *d;
    struct dirent *dir;
    d = opendir(pConf.base_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) 
            printf("%s/%s\n", pConf.base_path, dir->d_name);
        
        closedir(d);
    }

    return true;
}

extern "C" void app_main(void)
{
    PowerManagement::Enable();
    PowerLock lock(PowerLockType::MaxConfigured, "AppMain");
    lock.Aquire();
    ESP_LOGI(TAG, "CO2 Monitor Firmware\n");
    ESP_LOGI(TAG, "ESPIDF Version: %s\n", esp_get_idf_version());
   
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
   
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    MountSpiffs(conf);
    
    conf = {
      .base_path = "/web",
      .partition_label = "web",
      .max_files = 5,
      .format_if_mount_failed = true
    };
    
    MountSpiffs(conf);

    conf = {
      .base_path = "/dev",
      .partition_label = "dev",
      .max_files = 5,
      .format_if_mount_failed = true
    };
    
    MountSpiffs(conf);

    mainLoop = new MainLogicLoop();
   
    xTaskCreate(i2ctask, "i2ctask", 4096, NULL, 10, NULL);
    lock.Release();
}

