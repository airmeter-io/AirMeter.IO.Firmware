#pragma once


extern "C" {
    #define FONTS_TERMINUS_8X14_ISO8859_1 1
    
    #include <stdio.h>
    #include <assert.h>
    #include "esp_system.h"
    #include "esp_log.h"
    //#include "esp_vfs_dev.h"
    #include "freertos/FreeRTOS.h"
   // #include "esp_libc.h"
    #include "esp_task.h"    
    #include "freertos/task.h"
    #include <driver/gpio.h>
    #include "cJSON.h"
    #include <driver/i2c.h>
    #include <esp_err.h>
    #include <freertos/event_groups.h>
    #include "nvs.h"
    #include "nvs_flash.h"
   
    #include <esp_http_server.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/queue.h"
    #include "lwip/sockets.h"
    #include "lwip/err.h"
    #include <lwip/api.h>
    #include "esp_event_base.h"
    #include "mqtt_client.h"
    #include "lwip/apps/sntp.h"
    
    //#include "tcpip_adapter.h"
    #include "string.h"
    #include <sys/types.h>
    #include <sys/time.h>
    #include "esp_spiffs.h"
    #include "esp_task_wdt.h"
    #include "driver/gpio.h"
    #include "esp_partition.h"    

    #include "esp32/rom/ets_sys.h"
}

#define uint unsigned