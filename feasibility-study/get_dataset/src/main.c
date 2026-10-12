#include "wifi_secrets.h"
#include "esp_log.h" //Include le dichiarazioni necessarie per utilizzare il sistema di logging
#include "nvs_flash.h" //Include le dichiarazioni per gestire la non volatile storage, per conservare configurazioni e dati di inizializzazione
#include "esp_netif.h" //gestisce lo stack di rete TCP/IP
#include "esp_event.h" //inizializzazione sistema di gestione degli eventi
#include "esp_wifi.h"
#include <assert.h>

//static const char *TAG = "WIFI_SENSING";
/*
La funzione centrale che utilizzeremo sarà: esp_err_t esp_wifi_sta_get_ap_info(wifi_ap_record_t *ap_info);
-esp_err_t:	Tipo restituito dalla funzione, indica successo o errore
-esp_wifi_sta_get_ap_info:	Recupera le informazioni dell'Access Point connesso
-wifi_ap_record_t *ap_info	Puntatore alla struttura in cui vengono scritte le informazioni

Il campo target è: ap_info.rssi

L' RSSI rappresenta la potenza ricevuta dal router
*/

static const char *TAG = "WIFI_SENSING"; //puntatore statico 
// Struttura che contiene le informazioni della rete Wi-Fi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,          // Nome della rete
            .password = WIFI_PASSWORD   // Password della rete
        }
    };

/*
 * Event handler per gli eventi Wi-Fi.
 *
 * La funzione viene richiamata dal sistema di eventi
 * quando si verifica un evento a cui l'abbiamo registrata.
 */
static void wifi_event_handler(
    void *arg,                   
    esp_event_base_t event_base, 
    int32_t event_id,            
    void *event_data             
)
{
    //controllo che l' evento ricevuto appartenga alla famiglia wifi_event
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
    
        ESP_LOGI(TAG, "Wi-Fi avviato, connessione al router...");

        //richiesta di connessione all' AP configurato
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
    // Controllo di corretta connessione al router
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        ESP_LOGI(TAG, "ESP32 connesso al router!");
    }
    // Informazioni di disconnessione
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wifi_event_sta_disconnected_t *disconnected = (wifi_event_sta_disconnected_t *)event_data;
        ESP_LOGE(TAG, "Connessione Wi-Fi fallita/interrotta. Motivo: %d", disconnected->reason);
    }
}     

void app_main(void)
{
    //chiama la funzione di inizializzazione NVS e conserva nella variabile err l' output della funzione
    esp_err_t err = nvs_flash_init(); 

     if(err == ESP_OK)
         ESP_LOGI(TAG,"NVS inizializzata correttamente: %s",esp_err_to_name(err));
     else
     {
            ESP_LOGE(TAG, "Errore inizializzazione NVS: %s", esp_err_to_name(err));
            return;
     }

    //inizializzazione NETIF
    ESP_ERROR_CHECK(esp_netif_init()); //se OK va avanti nel flusso, se KO va in abort

    //inizializzazione del sistema di gestione degli eventi
    ESP_ERROR_CHECK(esp_event_loop_create_default()); /* //event loop, gestisce eventi dell' esp, es. connesso, disconnesso... */
    /*Event loop associa a questi elementi delle funzioni chiamate event handler*/

    /* sta_netif contiene l' indirizzo dell' interfaccia creata, la funzione crea e configura un' interfaccia di rete predefinita */
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    //controllo sul puntatore, se NULL, va in abort!
    assert(sta_netif != NULL);

    //inizializzazione del driver wifi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

     //registrazione della funzione che reagisce a ogni eventotramite ESP_EVENT_ANY_ID
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wifi_event_handler,NULL));

    /* configurazione del driver Wi-Fi in modalità station, cioè dispositivo che si connette ad un AP (Access Point) */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Configurazione della rete tramite associazione station mode con Wifi-Password
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));



    /*Avvio del driver wifi secondo la modalità precedentemente specificata*/
    ESP_ERROR_CHECK(esp_wifi_start());

}
