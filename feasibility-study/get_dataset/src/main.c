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

void app_main(void)
{
    //chiama la funzione di inizializzazione NVS e conserva nella variabile err l' output della funzione
    esp_err_t err = nvs_flash_init(); 

     if(err == ESP_OK)
         ESP_LOGE(TAG,"NVS inizializzata correttamente: %s",esp_err_to_name(err));
     else
     {
            ESP_LOGE(TAG, "Errore inizializzazione NVS: %s", esp_err_to_name(err));
            return 0;
     }

    //inizializzazione NETIF
    ESP_ERROR_CHECK(esp_netif_init()); //se OK va avanti nel flusso, se KO va in abort

    //inizializzazione del sistema di gestione degli eventi
    ESP_ERROR_CHECK(esp_event_loop_create_default()); /* //event loop, gestisce eventi dell' esp, es. connesso, disconnesso... */
    /*Event loop associa a questi elementi delle funzioni chiamate event handler*/

    /* //*sta_netif contiene l' indirizzo dell' interfaccia creata, la funzione crea e configura un' interfaccia di rete predefinita */
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    //controllo sul puntatore, se NULL, va in abort!
    assert(sta_netif != NULL);

    //inizializzazione del driver wifi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* configurazione del driver Wi-Fi in modalità station, cioè dispositivo che si connette ad un AP (Access Point) */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    /*Avvio del driver wifi secondo la modalità precedentemente specificata*/
    ESP_ERROR_CHECK(esp_wifi_start());

}
