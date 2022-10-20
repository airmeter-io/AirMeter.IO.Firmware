set(COMPONENT_SRCDIRS
  "thirdparty/BME280_driver" 
  "src"
)

set(COMPONENT_ADD_INCLUDEDIRS
  "thirdparty" 
  "include"
    "../../common"
)

idf_component_register(
  SRCS "src/BME280.cpp"
    "thirdparty/BME280_driver/bme280.c"
  INCLUDE_DIRS "include"
              "thirdparty"
              "../../common"
  REQUIRES "json"
          "nvs_flash"
          "I2C"
          "HTTPSERVER"
          "mqtt"
          "spiffs"
          "esp_http_server"
)