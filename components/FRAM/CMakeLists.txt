idf_component_register(
  SRCS "src/MB85FRAMManager.cpp"   
  INCLUDE_DIRS "include"
              "../../common"
  REQUIRES "json"
          "nvs_flash"
          "I2C"
          "mqtt"
          "spiffs"
          "esp_http_server"
          "ESP8266-RTOS-FONTS"
          "ESP8266-RTOS-SSD1306"
)