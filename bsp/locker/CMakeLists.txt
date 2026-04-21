idf_component_register( SRCS 
                            "locker.c"
                        INCLUDE_DIRS 
                            "."
                        REQUIRES
                            driver
                            freertos
                            esp_driver_uart
                            log
                            XST
                            nvs_flash
                        )