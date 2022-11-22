#include "stdafx.h"

#define UART_MODE       0
#define WIFI_MODE       1
#define BLUETOOTH_MODE  2
#define LORA_MODE       3

#define WIFI_SSID       SSID
#define WIFI_PASSWORD   PASSWORD
#define SERVER_URL      URL
#define PORT            5000

/**
 * Init connection
*/
void init_connector();

/**
 * Send data in type string
*/
void send_data(char* data);

/**
 * Send data in type integer
*/
void send_integer(int8_t data);