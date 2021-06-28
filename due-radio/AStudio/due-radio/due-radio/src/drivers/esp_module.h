#ifndef ESP_MODULE_H_
#define ESP_MODULE_H_

#include "common.h"
#include "utils/fifo.h"

/**
 * ESP module board driver
 *
 * This allows communication with ESP-8266 (running appropriate firmware)
 * connected through USART0 port.
 *
 * USART0: RX1 and TX1 on Arduino Due
 */

#ifndef ESP_RX_QUEUE_SIZE
#define ESP_RX_QUEUE_SIZE 512
#endif  // ESP_RX_QUEUE_SIZE

__EXTERN_C_BEGIN

extern uint8_t esp_rx_data;

extern uint8_t esp_rx_fifo_buff[ESP_RX_QUEUE_SIZE];
extern fifo_t esp_rx_fifo;

extern bool reading_raw_chunk;

typedef struct {
    //char* track_path;
    int32_t track_length_ms;
    int32_t sampling_frequency;
    int32_t total_chunks;
} track_info;

typedef struct {
    track_info track;
    int32_t current_chunk;
} currently_playing_info;

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minutes;
    int seconds;
} current_time;

void esp_module_hardware_setup(void);
bool esp_module_init(void);

bool esp_module_wifi_connect(const char* ssid, const char* password);
void esp_module_play_next(void);
void esp_module_play_previous(void);
currently_playing_info* esp_module_get_currently_playing(void);
track_info* esp_module_get_track_info(int track_id);
int esp_module_get_chunk(int track_id, int chunk_index);
void esp_module_get_next_chunk(void);
current_time* esp_module_get_current_time(void);
//void esp_module_start_stream(void);
//void esp_module_stop_stream(void);

void esp_module_tx_put_char(uint8_t value);
void esp_module_tx_put_line(const char* value);
void esp_module_tx_put_formatted(const char* format, ...)
    __attribute__((format (__printf__, 1, 0)));

bool esp_module_rx_read(void);
bool esp_module_rx_read_wait(void);
bool esp_module_rx_read_wait_timeout(const int32_t timeout_ms);
char* esp_module_rx_read_line(uint32_t max_length, int32_t timeout_ms);
bool esp_module_rx_char_ready(void);
void esp_module_rx_wait_until_char_ready(void);

void esp_module_clear_status(void);

void esp_module_rx_clear_queue(void);
uint8_t esp_module_rx_get_char(void);
uint8_t esp_module_rx_peek_char(void);

__EXTERN_C_END

#endif  // ESP_MODULE_H_
