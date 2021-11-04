idf_component_register(
    SRCS "src/SsdDisplay.cpp"        
    INCLUDE_DIRS "include"
                 "../../common"
    REQUIRES "json"
             "I2C")