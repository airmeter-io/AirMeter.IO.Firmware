

idf_component_register(
  SRCS "src/FontManager.cpp"
       "src/ScreenDefinition.cpp"
       "src/ScreenManager.cpp"
       "src/StringWithValues.cpp"
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
          "GPIOROUTING"
          "SENSORMANAGER"
          "QRCODE"
          "DRAWTARGET"
          "driver"
          "esp_wifi"
          
)