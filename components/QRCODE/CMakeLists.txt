idf_component_register(
  SRCS "src/QRCodeGenerator.cpp"
      "Thirdparty/QRCode/src/qrcode.c"
  INCLUDE_DIRS "include"
               "Thirdparty/QRCode/src"
                "../../common"
  REQUIRES "json"
           "nvs_flash"
           "mqtt"
           "spiffs"
           "SSD1306"
           "esp_http_server"
           "ESP8266-RTOS-FONTS"
           "ESP8266-RTOS-SSD1306")