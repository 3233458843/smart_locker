idf_component_register( SRCS 
                            "xst.c"
                        INCLUDE_DIRS 
                            "."
                        REQUIRES
                            driver
                            freertos
                            esp_driver_uart
                            log
                        )