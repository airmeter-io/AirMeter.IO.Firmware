

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
          "esp_adc"
          "esp_http_server"
          "UTF8"
          "driver"
          "esp_wifi"
)