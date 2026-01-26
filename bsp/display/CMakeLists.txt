idf_component_register( SRCS 
                            "display.c"
                            "touch.c"
                        INCLUDE_DIRS 
                            "."
                        REQUIRES
                            driver
                            
                    )