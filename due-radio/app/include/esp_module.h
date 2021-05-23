#ifndef ESP_MODULE_H_
#define ESP_MODULE_H_

#include "common.h"
#include "lcd.h"
#include "fifo.h"

/**
 * ESP module board driver
 *
 * This allows communication with ESP-8266 (running appropriate firmware)
 * connected through USART0 port.
 *
 * USART0: RX1 and TX1 on Arduino Due
 */

#ifndef ESP_RX_QUEUE_SIZE
#define ESP_RX_QUEUE_SIZE 128
#endif  // ESP_RX_QUEUE_SIZE

__EXTERN_C_BEGIN

extern uint8_t esp_rx_data;
extern lcd_t* default_lcd;

extern uint8_t esp_rx_fifo_buff[ESP_RX_QUEUE_SIZE];
extern fifo_t esp_rx_fifo;

extern bool stream_running;

void esp_module_hardware_setup(lcd_t* lcd_ptr);
bool esp_module_init(void);

bool esp_module_wifi_connect(const char* ssid, const char* password);
void esp_module_start_stream(void);
void esp_module_stop_stream(void);

void esp_module_tx_put_char(uint8_t value);
void esp_module_tx_put_line(const char* value);
void esp_module_tx_put_formatted(const char* format, ...)
    __attribute__((format (__printf__, 1, 0)));

bool esp_module_rx_read(void);
bool esp_module_rx_read_wait(void);
bool esp_module_rx_read_wait_timeout(const int32_t timeout_ms);
char* esp_module_rx_read_line(size_t max_length, int32_t timeout_ms);
bool esp_module_rx_char_ready(void);
void esp_module_rx_wait_until_char_ready(void);

void esp_module_clear_status(void);

void esp_module_rx_clear_queue(void);
uint8_t esp_module_rx_get_char(void);
uint8_t esp_module_rx_peek_char(void);

__EXTERN_C_END

#endif  // ESP_MODULE_H_
