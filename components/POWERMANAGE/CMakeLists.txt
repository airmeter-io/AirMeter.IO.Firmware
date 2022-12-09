idf_component_register(
  SRCS "src/PowerLock.cpp"
       "src/PowerManagement.cpp"
  INCLUDE_DIRS "include"
                "../../common"
  REQUIRES "json"
           "nvs_flash"
           "hal"
           "mqtt"
           "spiffs"
           "esp_http_server"
           "driver"
           "esp_wifi")