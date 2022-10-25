
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
#include "FontManager.h"
#include <tinyutf8/tinyutf8.h>
#include "DrawTarget.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    #include "EpdSpi.h"
    #include "SSD1680.h"
    #include "DEPG0213BN.h"
#endif


#define TAG "CO2 Monitor"

extern "C" {
    #include "esp_sleep.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    #include "soc/rtc.h"
    #include "soc/rtc_cntl_reg.h"
    #include "soc/efuse_reg.h"
    #include "esp_pm.h"
#endif    

    #include "freertos/xtensa_timer.h"
    
    #include "driver/gpio.h"
    
}




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
    _generalSettings = new GeneralSettings();
    _ramStore = new DataManagerRamTemporaryStore();
    _flashStore = new DataManagerFlashDataStore();
    _dataManager = new DataManager(_flashStore,_ramStore);
    _sensorManager = new SensorManager(*_generalSettings, *_i2c, *_dataManager);
    

    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0) 
    SSD1680 *ssd1680;
    DEPG0213BN *display;
    
    _io.init(4,false);

    ssd1680 = new SSD1680(_io);
    display = new DEPG0213BN(*ssd1680);
    _screenManager = new ScreenManager(display, *this, *_sensorManager, *this);   
    #else
    _screenManager = new ScreenManager(new NullDrawControl(), *this, *_sensorManager, *this);   
    #endif

    _voltageStr[0] = 0;

}

bool MainLogicLoop::ProcessOnUIThread() {
    bool requiresRedraw = false;

    if(_waitingForProvisioning && _wifi->IsProvisioned()) {
        _waitingForProvisioning = false;
        _screenManager->ChangeScreen("HOME");
    }

    if(_uiProcessCount%5 == 0) {
        auto voltage = _battery->GetBatteryVoltage();
        auto major = (int)voltage;
        auto minor = (int)((voltage - major)*100);
        snprintf(_voltageStr, sizeof(_voltageStr)-1, "%d.%dV", major, minor);
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
    _wifi = new Wifi(_generalSettings->GetDeviceName(), _generalSettings->GetApPassword());
    CommandHandler *command = nullptr;
    CaptiveRedirectHandler* captiveRedirect = nullptr;
    WebContentHandler *webContent = nullptr;
    

  //  _display = new Oledssd1306Display(*_generalSettings, _sensorManager->GetValues(), *_wifi, *_i2c );
    httpServer->Start();
 
    command = new CommandHandler(*_wifi, *_generalSettings, _sensorManager->GetValues(), *_dataManager);
    webContent = new WebContentHandler();
    httpServer->AddUrlHandler(webContent);
    httpServer->AddUrlHandler(command);
    SntpManager *sntp = nullptr;
    if(!_wifi->IsProvisioned()) {
        printf("Provisioning...\n");
        _screenManager->ChangeScreen("CAPTIVE");
        vTaskDelay(200 / portTICK_PERIOD_MS);
        captiveRedirect = new CaptiveRedirectHandler ();
        httpServer->AddUrlHandler(captiveRedirect);
        printf("Started provisioning-1\n");
        _wifi->StartProvisioning();
        
        _waitingForProvisioning = true;
        printf("Started provisioning-2 (delay)\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        printf("Provisioning started\n");
    } else {
    
        printf("Already provisioned starting WiFi...\n");

        sntp = new SntpManager(_generalSettings->GetNtpServers(), _generalSettings->GetEnableDhcpNtp());
        sntp->Init();
        _wifi->Start();

        if(_generalSettings->GetEnableMqtt()) {
            printf("Starting Mqtt...\n");
            _mqtt = new MqttManager(*_generalSettings,_sensorManager->GetValues());
        }
    }

    

    _sensorManager->UpdateValues();
   
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    esp_sleep_enable_wifi_wakeup();
#endif

   _screenManager->Run(5000 / portTICK_PERIOD_MS);

}

 std::string MainLogicLoop::ResolveValue(std::string pName) {
    if(pName == "CO2Ppm") { 
        if(_sensorManager->GetValues().CO2==nullptr)
            return "-";
        return std::to_string(_sensorManager->GetValues().CO2->GetPPM());
    }
    if(pName == "Temp")
        return _sensorManager->GetValues().Bme280.GetTemperatureStr();
    if(pName == "Pressure")
        return _sensorManager->GetValues().Bme280.GetPressureStr(2);
    if(pName == "Humidity")
        return _sensorManager->GetValues().Bme280.GetHumidityStr();
    if(pName == "Time") {
        
        time_t now;
        time(&now);
        char buf[sizeof "07:07:09Z"];
        strftime(buf, sizeof buf, "%H:%M", gmtime(&now));
        return buf;
    }
    if(pName == "BatVolts") {
        return _voltageStr;
    }

    if(pName == "AccessPointSSID") {
        return _generalSettings->GetDeviceName();
    }

    if(pName == "AccessPointPassword") {
        return _generalSettings->GetApPassword();
    }
    return "ERR";
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
        read = query->ReadEntries(entries, maxEntries);
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


 RTC_NOINIT_ATTR uint test;


extern "C" void app_main(void)
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    

#if CONFIG_IDF_TARGET_ESP32
    esp_pm_config_esp32_t pm_config = {
#elif CONFIG_IDF_TARGET_ESP32S2
    esp_pm_config_esp32s2_t pm_config = {
#elif CONFIG_IDF_TARGET_ESP32C3
    esp_pm_config_esp32c3_t pm_config = {
#elif CONFIG_IDF_TARGET_ESP32S3
    esp_pm_config_esp32s3_t pm_config = {
#endif
        .max_freq_mhz = 160,
        .min_freq_mhz = 40,
        .light_sleep_enable = true
    };
   // esp_pm_get_configuration(&pm_config);
    printf("Min = %d, max = %d, light=%s\n",(int)pm_config.min_freq_mhz, (int)pm_config.max_freq_mhz, pm_config.light_sleep_enable ? "yes" : "no");
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    
#endif

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
}

