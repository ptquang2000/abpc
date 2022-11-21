#include "stdafx.h"
#include "hal/i2c_types.h"
#include "vl53l1_platform_init.h"
#include "vl53l1_api.h"

#define MIN_SPADS_LEN				4
#define MAX_SPADS_LEN				16

typedef struct {
    VL53L1_Dev_t dev;
    VL53L1_DEV pdev;
    VL53L1_RangingMeasurementData_t measurement_data;
} Device;

/**
 *  Customized according to counting method 
*/
void setup_roi();

/**
 *  Init I2C for VL53L1X
*/
void config_tof(uint8_t i2c_slave_address, uint8_t comms_type, uint16_t comms_speed_khz);

/**
 *  Init VL53L1X mode
*/
void init_tof();

/**
 *  Get distance in next region of intertest has been setup.
*/
void switch_next_roi(int16_t* o_distances);

/**
 *  Print current data
*/
void print_distances(int16_t* i_distances);