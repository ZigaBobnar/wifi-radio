#include "esp_module.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t esp_rx_data = 0;

void esp_module_hardware_setup() {
    pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA10A_RXD0 | PIO_PA11A_TXD0);
    pio_pull_up(PIOA, PIO_PA10A_RXD0 | PIO_PA11A_TXD0, PIO_PULLUP);

    sysclk_enable_peripheral_clock(ID_USART0);
    sam_usart_opt_t usart_opts = {
        .baudrate = 115200,
        .char_length = US_MR_CHRL_8_BIT,
        .parity_type = US_MR_PAR_NO,
        .stop_bits = US_MR_NBSTOP_1_BIT,
    };
    usart_init_rs232(USART0, &usart_opts, sysclk_get_peripheral_hz());

    usart_enable_tx(USART0);
    usart_enable_rx(USART0);
}

bool esp_module_init() {
    uint8_t retries_left = 50;

    while (retries_left >= 0) {
        esp_module_clear_status();
        esp_module_write('s');

        if (esp_module_read_wait()) {
            if (esp_rx_data == 'O' && esp_module_read_wait()) {
                if (esp_rx_data == 'K') {
                    return true;
                }
            }
        }

        retries_left--;

        delay_ms(100);
    }

    return false;
}

bool esp_module_wifi_connect(const char* ssid, const char* password) {
    uint8_t ssid_length = strlen(ssid);
    uint8_t password_length = strlen(password);

    esp_module_clear_status();
    esp_module_write('c');

    esp_module_write(ssid_length);
    esp_module_write(password_length);

    for (uint8_t i = 0; i < ssid_length; i++) {
        esp_module_write(ssid[i]);
    }

    for (uint8_t i = 0; i < password_length; i++) {
        esp_module_write(password[i]);
    }

    if (esp_module_read_wait_timeout(1000000) && esp_rx_data == 0x01) {
        // Success
        return true;
    } else {
        return false;
    }
}

void esp_module_start_stream() {
    esp_module_clear_status();
    esp_module_write('S');
}

void esp_module_stop_stream() {
    esp_module_clear_status();
    esp_module_write('E');
    esp_module_clear_status();
}

bool esp_module_read() {
    return (usart_read(USART0, &esp_rx_data) == 0);
}

bool esp_module_read_wait() {
    return esp_module_read_wait(10000);
}

bool esp_module_read_wait_timeout(const uint32_t retries_timeout) {
    uint32_t retries_left = retries_timeout;

    while (usart_read(USART0, &esp_rx_data) != 0) {
        retries_left--;

        if (retries_left <= 0) {
            return false;
        }
    }

    return true;
}

void esp_module_write(uint8_t value) {
    usart_write(USART0, value);
}

void esp_module_clear_status() {
    // TODO: This probably does not clear the rx queue...
    usart_reset_status(USART0);
}

#ifdef __cplusplus
}
#endif
