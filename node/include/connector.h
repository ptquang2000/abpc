#include "stdafx.h"

#define UART        0
#define WIFI        1
#define BLUETOOTH   2
#define LORA        3

void init_connector();
void send_data(char* data);
void send_integer(int8_t data);