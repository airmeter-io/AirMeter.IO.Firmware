idf_component_register(
  SRCS "src/SSD1680.cpp"
    "src/EPDBackBuffer.cpp"
    "src/EPDDrawTarget.cpp"
    "src/DrawControlSSD16xx.cpp"
    "src/DriverSSD16xx.cpp"    
    "src/epdspi.cpp"
  INCLUDE_DIRS "include"
              "../../common"
  REQUIRES "json"
          "nvs_flash"
          "I2C"
          "esp_timer"
          "HTTPSERVER"
          "mqtt"
          "spiffs"
          "esp_http_server"
          "GPIOROUTING"
          "BITMAPFONT"
          "SCREENMANAGER"
          "driver"
          "esp_wifi"
)