/*
    *   Task per leggere l' RSSI 
    *   Uso un task FREERTOS per poter andare a parallelizare,
    *   i job, esp32 è a single core, quindi bisogna orchestrare
    *   l' uso del single-core
    *   Il task viene quindi gestito dallo scheduler di FreeRTOS
*/
#include "rssi_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"

static const char *TAG = "RSSI_TASK";

static void rssi_task(void *pvParameters)
{
    wifi_ap_record_t ap_info;

    while(1)
    {
        esp_err_t err = esp_wifi_sta_get_ap_info(&ap_info);

        if (err == ESP_OK)
        {
            ESP_LOGI(TAG, "RSSI: %d dBm", ap_info.rssi);
        }
        else
        {
            ESP_LOGW(TAG, "Impossibile leggere RSSI");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void rssi_task_start(void){
    BaseType_t result = xTaskCreate(
        rssi_task,
        "rssi_task",
        4096,
        NULL,
        5,
        NULL
    );

    if (result != pdPASS)
    {
        ESP_LOGE(TAG, "Creazione task fallita");
    }
}