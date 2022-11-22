#include "connector.h"
#include "driver/uart.h"

#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_http_client.h"

#define MAX_DATA_SIZE   256

static const char* TAG = "connector";
static const char* NODE = STR(NODE_NAME);
static char s_data[MAX_DATA_SIZE];
TaskHandle_t send_data_handle;

/**
 * Init protocol function
*/
void init_uart();
void init_wifi();
void init_bluetooth();
void init_lora();

/**
 * Transmit data function
*/
void send_data_by_uart();
void send_data_by_wifi();
void send_data_by_bluetooth();
void send_data_by_lora();

void send_data_task(void* pvParameters) {
    uint32_t ulTaskNotifiedValue;
    while (1) {
        if (xTaskNotifyWait((1<<0)|(1<<1), 0, &ulTaskNotifiedValue, portMAX_DELAY) == pdTRUE)
        {
            switch (CONN_TYPE)
            {
            case UART_MODE: send_data_by_uart(); break;
            case WIFI_MODE: send_data_by_wifi(); break;
            case BLUETOOTH_MODE: send_data_by_bluetooth(); break;
            case LORA_MODE: send_data_by_lora(); break;
            default: break;
            }
        }
    }
}

void init_connector() {
    (void) TAG;
    switch (CONN_TYPE)
    {
    case UART_MODE: init_uart(); break;
    case WIFI_MODE: init_wifi(); break;
    case BLUETOOTH_MODE: init_bluetooth(); break;
    case LORA_MODE: init_lora(); break;
    default: break;
    }
    xTaskCreate(&send_data_task, "send_data_task", 4096, NULL, tskIDLE_PRIORITY, &send_data_handle);
}

void send_integer(int8_t i_value) {
    sprintf(s_data, "%d", i_value);
    xTaskNotify(send_data_handle, (1<<1), eSetBits);
}

void send_data(char* i_data) {
    memcpy(s_data, i_data, strlen(i_data));
    xTaskNotify(send_data_handle, (1<<0), eSetBits);
}

#if CONN_TYPE == UART_MODE

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

void send_data_by_uart() {
    char* cmd = malloc(strlen(NODE) + strlen(s_data) + 12);
    sprintf(cmd, "!NODE:%s#\n", NODE);
    uart_write_bytes(UART_NUM_2, cmd, strlen(cmd));
    sprintf(cmd, "!QUANTITY:%s#\n", s_data);
    uart_write_bytes(UART_NUM_2, cmd, strlen(cmd));
    free(cmd);
}

#elif CONN_TYPE == WIFI_MODE

#define CONFIG_ESP_WIFI_SSID        WIFI_SSID
#define CONFIG_ESP_WIFI_PASSWORD    WIFI_PASSWORD
#define CONFIG_ESP_MAXIMUM_RETRY    5
#define WIFI_CONNECTED_BIT          BIT0
#define WIFI_FAIL_BIT               BIT1

#define CONFIG_ESP_WIFI_AUTH_OPEN   1
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < CONFIG_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        (void)event;
        // ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void init_wifi() {
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        NULL,
        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        NULL,
        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY);
    ESP_ERROR_CHECK((bits & WIFI_CONNECTED_BIT) ^ 1);
    ESP_ERROR_CHECK(bits & WIFI_FAIL_BIT);
}

esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR: break;
        case HTTP_EVENT_ON_CONNECTED: break;
        case HTTP_EVENT_HEADER_SENT: break;
        case HTTP_EVENT_ON_HEADER: break;
        case HTTP_EVENT_ON_DATA:
        {
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // printf("%.*s", evt->data_len, (char*)evt->data);
            }
        }
        break;
        case HTTP_EVENT_ON_FINISH: break;
        case HTTP_EVENT_DISCONNECTED: break;
    }
    return ESP_OK;
}

void send_data_by_wifi() {
    char* url = malloc((sizeof SERVER_URL) + strlen(NODE) + strlen(s_data) + 16);
    sprintf(url, "%s?NODE=%s&QUANTITY=%s", SERVER_URL, NODE, s_data);
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handle,
        .port = PORT
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
    free(url);
}

#else
#error This protocol has not been supported yet
#endif