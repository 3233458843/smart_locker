idf_component_register( SRCS 
                            "display.c"
                            "touch.c"
                        INCLUDE_DIRS 
                            "."
                        REQUIRES
                            driver
                            esp_lcd 
                            esp_driver_spi
                            esp_driver_i2c
                            esp_driver_ledc
                            freertos
                            log
                    )