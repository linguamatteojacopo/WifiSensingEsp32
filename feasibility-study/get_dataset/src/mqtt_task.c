#include "mqtt_task.h"
#include "mqtt_client.h"
#include "mqtt_secrets.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdbool.h>

static const char *TAG = "MQTT_TASK";

static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_connected = false; // true se coonessione è OK, false se connessione non è OK

//Gestione degli eventi MQTT
static void mqtt_event_handler(void *handler_args,esp_event_base_t base,int32_t event_id,void *event_data)
{
    switch (event_id)
    {
    case MQTT_EVENT_CONNECTED:
        mqtt_connected = true;
        ESP_LOGI(TAG, "Connesso al broker MQTT!");
        break;
    
    case MQTT_EVENT_DISCONNECTED:
        mqtt_connected=false;
        ESP_LOGW(TAG, "Disconnesso dal broker MQTT!");
        break;
    
    default:
        ESP_LOGW(TAG,"Nessun evento rilevato con il broker!");
        break;
    }
}

//inizializzazione client MQTT
void mqtt_init(void){
    //configurazione broker MQTT
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "Inizializzazione MQTT fallita");
        return;
    }

    //Inizializzazione gestore degli eventi MQTT (definito sopra)
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(mqtt_client,ESP_EVENT_ANY_ID,mqtt_event_handler,NULL));

    //start client MQTT
    ESP_ERROR_CHECK(esp_mqtt_client_start(mqtt_client));
}

//Pubblicazione campione RSSI
esp_err_t mqtt_publish_rssi(int rssi, uint32_t uptime_ms){

    if (mqtt_client == NULL || !mqtt_connected)
        return ESP_ERR_INVALID_STATE;

    char payload[100];

    snprintf(payload,sizeof(payload),"{\"rssi\":%d,\"uptime_ms\":%lu}",rssi,(unsigned long)uptime_ms);

    //inviamo con Qos = 0 per le prime prove
    int msg_id = esp_mqtt_client_publish(mqtt_client,MQTT_TOPIC,payload,0,0,0); 

    if (msg_id < 0)
    {
        ESP_LOGE(TAG, "Pubblicazione MQTT fallita");
        return ESP_FAIL;
    }

    return ESP_OK;
}
