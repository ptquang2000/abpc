#include "apc.h" 

#include "stdint.h"

#include "ClassADevice.h"
#include "LoraDevice.h"

#include "esp_log.h"

#include "FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#define CFG_START_COUNT_BUTTON 16
#define CFG_DISTANCE_THRESHOLD 1600 // mm


const static char* TAG = "ABPC";

static uint8_t app_key[APP_KEY_SIZE] = {0x96, 0xf4, 0x15, 0x5e, 0xfa, 0x37, 0xbe, 0xb7, 0x60, 0x5e, 0x4d, 0x5d, 0x6d, 0x65, 0x64, 0xe8};;
static uint8_t join_eui[JOIN_EUI_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t dev_eui[DEV_EUI_SIZE] = {0xFE, 0xFF, 0xFF, 0x00, 0x00, 0x0A, 0xAA, 0xAA};
static uint16_t dev_nonce = 0;

static int s_count = 0;
TaskHandle_t btn_task_handle = NULL;

void btn_task(void *p)
{
    int btn_state = 0;
    int last_btn_state = btn_state;
    int count_state = 0;
    while (1)
    {
        btn_state = gpio_get_level(CFG_START_COUNT_BUTTON);
        if (btn_state != last_btn_state)
        {
           last_btn_state = btn_state;
           if (btn_state)
           {
               ESP_LOGI(TAG, "Button pressed");
               count_state++;
               count_state = count_state % 2;
               if (count_state)
               {
                   ESP_LOGI(TAG, "Start counting");
                   APC_start_count(CFG_DISTANCE_THRESHOLD);
               }
               else
               {
                   APC_stop_count();
                   // ClassADevice_send_data_unconfirmed(data, sizeof(data), 10);
                   // ClassADevice_send_data_confirmed(data, sizeof(data), 10);
               }
           }
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL << CFG_START_COUNT_BUTTON;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    LoraDevice* device = LoraDevice_create(app_key, join_eui, dev_eui, dev_nonce);

    ClassADevice_intialize(device);
    // ClassADevice_register_event();
    // ClassADevice_connect();

    APC_create();
    APC_config conf = {
        .roi_x = 8,
        .roi_y = 16,
        .timing_budget = Ms20,
        .distance_mode = LongDistanceMode,
    };
    APC_initialize(&conf);

    xTaskCreate(btn_task, "btn_task", 1024, NULL, tskIDLE_PRIORITY, &btn_task_handle);
    int last_count = 0;
    while (1)
    {
        s_count = APC_get_count();
        if (last_count != s_count)
        {
            ESP_LOGI(TAG, "People count: %d", s_count);
            last_count = s_count;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
