#include "stdafx.h"
#include "nvs_flash.h"

static const char* TAG = "main";

#if defined(USE_TWO_ROI)
void two_roi();
#elif defined(USE_MULTIPLE_ROI)
void multiple_roi();
#endif

void app_main() {
    (void) TAG;
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    two_roi();
}