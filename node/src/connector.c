#include "connector.h"
#include "driver/uart.h"

#define DATA_SIZE   260

void init_uart();
void init_wifi();
void init_bluetooth();
void init_lora();

void send_data_by_uart(char* data);
void send_data_by_wifi(char* data);
void send_data_by_bluetooth(char* data);
void send_data_by_lora(char* data);

QueueHandle_t uart_queue;

void init_uart() {
    const uint16_t bufferSize = (1024 * 2);
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, bufferSize, bufferSize, 10, &uart_queue, 0));
    uart_config_t uart_config = {
        .baud_rate = BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, TX_IO_NUM, RX_IO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

void send_data_by_uart(char* data) {
    uart_write_bytes(UART_NUM_2, data, DATA_SIZE);
}

void init_connector() {
    switch (CONN_TYPE)
    {
    case UART: init_uart(); break;
    case WIFI: init_wifi(); break;
    case BLUETOOTH: init_bluetooth(); break;
    case LORA: init_lora(); break;
    default: break;
    }
}

void formatted_integer(int8_t value, char *o_data) {
    char data[DATA_SIZE];
    sprintf(data, "!%256d#\n", value);
    memcpy(o_data, data, strlen(data));
}

void send_integer(int8_t value) {
    char data[DATA_SIZE];
    formatted_integer(value, data);
    send_data(data);
}

void send_data(char* value) {
    switch (CONN_TYPE)
    {
    case UART: send_data_by_uart(value); break;
    case WIFI: send_data_by_wifi(value); break;
    case BLUETOOTH: send_data_by_bluetooth(value); break;
    case LORA: send_data_by_lora(value); break;
    default: break;
    }
}