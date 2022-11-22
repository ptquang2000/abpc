#pragma once

#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h" 
#include "esp_log.h"

#define ST(A) #A
#define STR(A) ST(A)