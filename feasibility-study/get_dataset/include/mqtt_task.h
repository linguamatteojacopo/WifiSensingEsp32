
#ifndef MQTT_TASK_H
#define MQTT_TASK_H

#include <stdint.h>
#include "esp_err.h"

void mqtt_init(void); //inizializzazione MQTT 
esp_err_t mqtt_publish_rssi(int rssi, uint32_t uptime_ms); //PUBBLICAZIONE RSSI

#endif
