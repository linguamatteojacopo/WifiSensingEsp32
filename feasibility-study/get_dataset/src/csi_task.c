#include "csi_task.h"
#include "esp_wifi.h"
#include "esp_log.h"

static const char *TAG = "CSI_TASK";
static unsigned long sample_count = 0;

static void csi_rx_callback(void *ctx, wifi_csi_info_t *info)
{
    if (info == NULL || info->buf == NULL || info->len == 0) {
        return;
    }

    sample_count++;

    if (sample_count % 100 == 0) {
        ESP_LOGI(TAG,
                 "CSI ricevuto: campione=%lu, RSSI=%d, len=%d",
                 sample_count,
                 info->rx_ctrl.rssi,
                 info->len);
    }
}

esp_err_t csi_init(void)
{
    wifi_csi_config_t config = {
        .enable = true,
        .acquire_csi_legacy = false,
        .acquire_csi_ht20 = true,
        .acquire_csi_ht40 = true,
        .acquire_csi_su = true,
        .acquire_csi_mu = true,
        .acquire_csi_dcm = true,
        .acquire_csi_beamformed = true,
        .acquire_csi_he_stbc = 2,
        .val_scale_cfg = false,
        .dump_ack_en = false,
        .reserved = false
    };

    esp_err_t err;

    ESP_LOGI(TAG, "Configurazione CSI...");

    err = esp_wifi_set_csi_config(&config);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_csi_config fallita: %s (0x%x)",
                 esp_err_to_name(err), (unsigned int)err);
        return err;
    }

    ESP_LOGI(TAG, "Configurazione CSI riuscita");

    err = esp_wifi_set_csi_rx_cb(csi_rx_callback, NULL);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_csi_rx_cb fallita: %s (0x%x)",
                 esp_err_to_name(err), (unsigned int)err);
        return err;
    }

    ESP_LOGI(TAG, "Callback CSI registrata");

    err = esp_wifi_set_csi(true);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_csi fallita: %s (0x%x)",
                 esp_err_to_name(err), (unsigned int)err);
        return err;
    }

    ESP_LOGI(TAG, "CSI abilitato correttamente");

    return ESP_OK;
}