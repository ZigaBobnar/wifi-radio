#ifndef ESP_MODULE_H_
#define ESP_MODULE_H_

#include "asf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is a simple driver for ESP-8266 module connected through USART0 port.
 * USART0 => pins RX1, TX1 on Arduino Due
 */

extern uint32_t esp_rx_data;

void esp_module_hardware_setup(void);
bool esp_module_init(void);
bool esp_module_wifi_connect(const char* ssid, const char* password);
void esp_module_start_stream(void);
void esp_module_stop_stream(void);
bool esp_module_read(void);
bool esp_module_read_wait(void);
bool esp_module_read_wait_timeout(const uint32_t retries_timeout);
void esp_module_write(uint8_t value);
void esp_module_clear_status(void);

#ifdef __cplusplus
}
#endif

#endif  // ESP_MODULE_H_
