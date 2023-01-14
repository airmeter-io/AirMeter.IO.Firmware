

idf_component_register(
  SRCS "src/UserManager.cpp"
  INCLUDE_DIRS "include"
              "../../common"
  REQUIRES "json"
          "nvs_flash"
          "I2C"
          "HTTPSERVER"
          "mqtt"
          "spiffs"
          "esp_http_server"
          "BITMAPFONT"
          "EPD"
          "GPIOROUTING"
          "CO2SENSORS"
          "VALUES"
          "BME280"
          "DATAMANAGER"
          "driver"
          "esp_wifi"
)