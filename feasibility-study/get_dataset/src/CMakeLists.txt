idf_component_register(SRCS "main.c"
                            "rssi_task.c"
                       INCLUDE_DIRS 
                            "../include"
                       REQUIRES mqtt
                       )