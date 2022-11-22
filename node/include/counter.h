#include "stdafx.h"

#if defined(USE_TWO_ROI)
/**
 * Get current region of interest
*/
int8_t get_region();

/**
 * Get movement pattern
*/
int8_t* get_movement();
#endif

/**
 * Get current quantity
*/
int8_t get_count();

/**
 * Check for update new quantiy
*/
void check_count(int16_t* distances);
