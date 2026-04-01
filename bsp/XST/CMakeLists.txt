idf_component_register( SRCS 
                            "xst.c"
                        INCLUDE_DIRS 
                            "."
                        REQUIRES
                            driver
                            freertos
                            lwip
                            esp_driver_uart
                            log
                        )