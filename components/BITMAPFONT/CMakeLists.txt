

idf_component_register(
  SRCS "src/BitmapFont.cpp"
  INCLUDE_DIRS "include"
              "../../common"
  REQUIRES "json"
          "nvs_flash"
          "I2C"
          "HTTPSERVER"
          "mqtt"
          "spiffs"
          "esp_http_server"
          "UTF8"
          "DRAWTARGET"
          "driver"
          "esp_wifi"
)