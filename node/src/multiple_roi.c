#include "stdafx.h"
#include "connector.h"
#include "tof.h"
#include "interpolate.h"
#include "counter.h"

static const char* TAG = "multiple_roi";

extern VL53L1_UserRoi_t roi_config[NUM_OF_CENTER];
extern Device device;
static int16_t distances[NUM_OF_CENTER];
static float intdistances[INTERPOLATE_NUM];
static TaskHandle_t get_distance_handle = NULL;

#ifdef USER_MULTIPLE_ROI
void setup_roi() {
    uint8_t i = 0;
	for (uint8_t y = 0; y < MIN_SPADS_LEN; y++) {
		for (uint8_t x = 0; x < MIN_SPADS_LEN; x++) {
            VL53L1_UserRoi_t roiConfig =  {
                .TopLeftX = 4*x,
	            .TopLeftY = (15-4*y),
	            .BotRightX = (4*x+3),
	            .BotRightY = (15-4*y-3)
                };
			roi_config[i] = roiConfig;
			i++;
		}
	}
}
#endif
static void get_distance_task(void *pvParameters) {
    while (1) {
        switch_next_roi(distances);
        float f_dinstaces[NUM_OF_CENTER];
        for(int i = 0; i < NUM_OF_CENTER; ++i) {
            f_dinstaces[i] = (float)distances[i];
        }
        interpolate_image(f_dinstaces, 4, 4, intdistances, 8, 8);
        send_integer(get_count());
    }
}

void multiple_roi() {
    (void)TAG;

    config_tof(0x29, VL53L1_I2C, (uint16_t)I2C_FREQ_HZ);
    init_tof();
    init_connector();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    xTaskCreate(&get_distance_task, "get_distance_task", 4096, (void*)&device, tskIDLE_PRIORITY + 1, &get_distance_handle);

    while (1) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}