idf_component_register(
  SRCS "src/DrawTarget.cpp"
  INCLUDE_DIRS "include"
              "../../common"
  REQUIRES "json"
          "nvs_flash"
          "I2C"
          "HTTPSERVER"
          "SOFTSERIAL"
          "mqtt"
          "spiffs"
          "esp_http_server"
          "driver"
          "esp_wifi"
)