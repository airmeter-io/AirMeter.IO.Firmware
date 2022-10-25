idf_component_register(
  SRCS "src/SntpManager.cpp"
  INCLUDE_DIRS "include"
                "../../common"
  REQUIRES "json"
           "nvs_flash"
           "mqtt"
           "spiffs"
           "esp_http_server"
           "driver"
           "esp_wifi")