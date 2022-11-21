#include "stdafx.h"
#include "connector.h"
#include "tof.h"
#include "counter.h"

static const char* TAG = "two_roi";

extern VL53L1_UserRoi_t roi_config[NUM_OF_CENTER];
extern Device device;
static int16_t distances[NUM_OF_CENTER];
static TaskHandle_t get_distance_handle = NULL;

#ifdef USE_TWO_ROI
void setup_roi() {
    VL53L1_UserRoi_t config;

    config.TopLeftX = 8;
    config.TopLeftY = 15;
    config.BotRightX = 15;
    config.BotRightY = 0;
    roi_config[0] = config;

    config.TopLeftX = 4;
    config.TopLeftY = 15;
    config.BotRightX = 11;
    config.BotRightY = 0;
    roi_config[1] = config;
}
#endif

static void get_distance_task(void *pvParameters) {
    while (1) 
    {
        switch_next_roi(distances);
        check_count(distances);
        send_integer(get_count());
    }
}

void two_roi() {
    (void) TAG;

    for (uint8_t i = 0; i < NUM_OF_CENTER; i++)
    {
        distances[i] = DISTANCE_THRESHOLD;
    }

    init_connector();    
    config_tof(0x29, VL53L1_I2C, (uint16_t)I2C_FREQ_HZ);
    init_tof();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    xTaskCreate(&get_distance_task, "get_distance_task", 4096, (void*)&device, tskIDLE_PRIORITY + 1, &get_distance_handle);

    while (1) 
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}