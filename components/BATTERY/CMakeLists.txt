

idf_component_register(
  SRCS "src/BatteryManager.cpp"
  INCLUDE_DIRS "include"
              "../../common"
  REQUIRES "json"
          "nvs_flash"
          "I2C"
          "HTTPSERVER"
          "mqtt"
          "spiffs"
          "esp_adc_cal"
          "esp_http_server"
          "ESP8266-RTOS-FONTS"
          "ESP8266-RTOS-SSD1306"
          "UTF8"
)