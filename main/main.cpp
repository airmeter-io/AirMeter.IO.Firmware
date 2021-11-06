
#include "main.h"

#include "I2C.h"
#include "OledSsd1306Display.h"
#include "HttpServer.h"
#include "CaptiveRedirectHandler.h"
#include "WebContentHandler.h"
#include "CommandHandler.h"
#include "SntpManager.h"
#include "MqttManager.h"
#include <dirent.h> 
#include "PasswordGenerator.h"
#include "DataManagerRamTemporaryStore.h"
#include "DataManagerFlashDataStore.h"

#define TAG "CO2 Monitor"

extern "C" {
    #include "esp_sleep.h"
}


#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
void print_wakeup_reason(){
	auto wakeup_reason = esp_sleep_get_wakeup_cause();
	switch(wakeup_reason)
	{
		case  ESP_SLEEP_WAKEUP_EXT0  : printf("Wakeup caused by external signal using RTC_IO\n"); break;
		case ESP_SLEEP_WAKEUP_EXT1  : printf("Wakeup caused by external signal using RTC_CNTL\n"); break;
		case ESP_SLEEP_WAKEUP_TIMER  : printf("Wakeup caused by timer\n"); break;
		case ESP_SLEEP_WAKEUP_TOUCHPAD  : printf("Wakeup caused by touchpad\n"); break;
		case ESP_SLEEP_WAKEUP_ULP  : printf("Wakeup caused by ULP program\n"); break;
		default : printf("Wakeup was not caused by deep sleep %d\n",wakeup_reason); break;
	}
}
#endif

void MainLogicLoop::ProcessEvents() {
    _buttons->ProcessQueue();
    _sensorManager->UpdateValues();
}

void MainLogicLoop::DisplayAPAuthInfo(const std::string& pSSID, const std::string& pPassword) {
    _display->RenderAccessPointInfo(pSSID, pPassword);
}


void MainLogicLoop::Run() {
    I2C i2c(GPIO_NUM_4, GPIO_NUM_5);
    _buttons = new ButtonManager();
    printf("Hello!\n");
    i2c.Scan();
    
    auto httpServer = new HttpServer();
    _generalSettings = new GeneralSettings();

    if(_generalSettings->GetApPassword().length()==0) {
        PasswordGenerator pwGen;
        _generalSettings->SetApPassword(pwGen.Generate(24));
        _generalSettings->Save();
    }
    Wifi wifi(_generalSettings->GetDeviceName(), _generalSettings->GetApPassword(), *this);
    CommandHandler *command = nullptr;
    CaptiveRedirectHandler* captiveRedirect = nullptr;
    WebContentHandler *webContent = nullptr;
    MqttManager* mqtt = nullptr;
    
    auto ramStore = new DataManagerRamTemporaryStore();
    auto flashStore = new DataManagerFlashDataStore();
    auto dataManager = new DataManager(flashStore,ramStore);
    
    _sensorManager = new SensorManager(*_generalSettings, i2c, *dataManager);
    
    _display = new Oledssd1306Display(*_generalSettings, _sensorManager->GetValues(), wifi, i2c );
    httpServer->Start();
 
    command = new CommandHandler(wifi, *_generalSettings, _sensorManager->GetValues(), *dataManager);
    webContent = new WebContentHandler();
    httpServer->AddUrlHandler(webContent);
    httpServer->AddUrlHandler(command);
    SntpManager *sntp = nullptr;
    if(!wifi.IsProvisioned()) {
        printf("Provisioning...\n");
        
        captiveRedirect = new CaptiveRedirectHandler ();
        httpServer->AddUrlHandler(captiveRedirect);
   
        wifi.StartProvisioning();
        vTaskDelay(2000 / portTICK_RATE_MS);
    } else {
        printf("Already provisioned starting WiFi...\n");

        sntp = new SntpManager(_generalSettings->GetNtpServers(), _generalSettings->GetEnableDhcpNtp());
        sntp->Init();
        wifi.Start();

        if(_generalSettings->GetEnableMqtt()) {
            printf("Starting Mqtt...\n");
            mqtt = new MqttManager(*_generalSettings,_sensorManager->GetValues());
        }
    }

    _sensorManager->UpdateValues();
    // while(true) {
    //     _display->RenderReadings();
    //     vTaskDelay(2000 / portTICK_RATE_MS);
    // }
    
    bool gotNtp = false;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    esp_sleep_enable_wifi_wakeup();
#endif
    while (true) {
        _display->RenderReadings();
        // printf("Slept \n");
        // esp_sleep_enable_timer_wakeup(100000000 );
        // esp_light_sleep_start() ;
        // printf("Woken up\n");
        //  print_wakeup_reason();
        vTaskDelay(1000 / portTICK_RATE_MS);
        
        ProcessEvents();

        if(mqtt!=nullptr) mqtt->Tick();

        if(!gotNtp) {
            for(auto i = 0; i< LWIP_DHCP_MAX_NTP_SERVERS; i++) {
                if(sntp_getserver(i)!=0) {
                    ESP_LOGI(TAG,"NTP %s\n", ipaddr_ntoa(sntp_getserver(i)));
                    gotNtp = true;
                }
            }
        }
        
        
        
        
    }

}

static void i2ctask(void *arg)
{
	MainLogicLoop main;
    main.Run();
	vTaskDelete(NULL);
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
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    print_wakeup_reason();
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
  

    xTaskCreate(i2ctask, "i2ctask", 4096, NULL, 10, NULL);
}

