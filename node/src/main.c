#include "stdafx.h"
static const char* TAG = "main";

#if defined(USE_TWO_ROI)
void two_roi();
#elif defined(USE_MULTIPLE_ROI)
void multiple_roi();
#endif

void app_main() {
    (void) TAG;
    two_roi();
}