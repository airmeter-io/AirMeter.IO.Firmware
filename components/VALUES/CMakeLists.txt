idf_component_register(
    SRCS "src/ValueController.cpp"        
    INCLUDE_DIRS "include"
                 "../../common"
    REQUIRES "json"
             "I2C"
             "driver")