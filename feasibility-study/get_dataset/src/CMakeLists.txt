idf_component_register(SRCS "main.c"
                            "rssi_task.c"
                             "mqtt_task.c"
                             "csi_task.c"
                       INCLUDE_DIRS 
                            "../include"
                            "../.pio"
                       REQUIRES 
                              mqtt
                              esp_timer
                       )