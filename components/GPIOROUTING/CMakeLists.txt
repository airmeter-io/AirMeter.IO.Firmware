idf_component_register(
  SRCS "src/GpioManager.cpp"
    "src/ButtonManager.cpp"    
  INCLUDE_DIRS "include"
              "../../common"
  REQUIRES "json"
          "nvs_flash"
          "mqtt"
          "spiffs"
          "esp_http_server"
          "ESP8266-RTOS-FONTS"
          "ESP8266-RTOS-SSD1306"
)