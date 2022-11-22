#include "tof.h"
#include "driver/i2c.h"

static const char* TAG = "tof";

Device device;
VL53L1_UserRoi_t roi_config[NUM_OF_CENTER];

static uint8_t status = ESP_OK;

void print_distances(int16_t* i_distances) {
    for (int8_t i = 0; i < NUM_OF_CENTER; i++)
    {
        printf("%5d ", i_distances[i]);
    }
    printf("\n");
}

void config_tof(uint8_t i2c_slave_address, uint8_t comms_type, uint16_t comms_speed_khz) {
    device.dev.i2c_slave_address = i2c_slave_address;
    device.dev.comms_type = comms_type;
    device.dev.comms_speed_khz = comms_speed_khz;
    device.pdev = &device.dev;

    VL53L1_platform_init(
        &device.dev, 
        device.dev.i2c_slave_address, 
        device.dev.comms_type, 
        device.dev.comms_speed_khz
    );

    setup_roi();
}

void init_tof() {
    (void) TAG;

    status += VL53L1_WaitDeviceBooted(device.pdev);
    status += VL53L1_DataInit(device.pdev);
	status += VL53L1_StaticInit(device.pdev);
	status += VL53L1_SetDistanceMode(device.pdev, VL53L1_DISTANCEMODE_MEDIUM);
	status += VL53L1_SetMeasurementTimingBudgetMicroSeconds(device.pdev, 20000);
	status += VL53L1_SetInterMeasurementPeriodMilliSeconds(device.pdev, 24);
    ESP_ERROR_CHECK(status);
    VL53L1DevDataSet(device.pdev, LLData.measurement_mode, VL53L1_DEVICEMEASUREMENTMODE_SINGLESHOT);
	VL53L1_StartMeasurement(device.pdev);
}

void switch_next_roi(int16_t* o_distances) {
    static uint8_t roi_index = 0;
    ESP_ERROR_CHECK(VL53L1_SetUserROI(device.pdev, &roi_config[roi_index]));
    ESP_ERROR_CHECK(VL53L1_WaitMeasurementDataReady(device.pdev));
    if (!status) status = VL53L1_GetRangingMeasurementData(device.pdev, &device.measurement_data);
    o_distances[roi_index] = device.measurement_data.RangeMilliMeter;
    ESP_ERROR_CHECK(VL53L1_ClearInterruptAndStartMeasurement(device.pdev));
    roi_index = roi_index == NUM_OF_CENTER - 1 ? 0 :  roi_index + 1;
}