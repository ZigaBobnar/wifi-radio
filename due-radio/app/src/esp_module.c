#include "esp_module.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t esp_rx_data = 0;
lcd_t* default_lcd = NULL;

void esp_module_hardware_setup(lcd_t* lcd_ptr) {
    default_lcd = lcd_ptr;

    pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA10A_RXD0 | PIO_PA11A_TXD0);
    pio_pull_up(PIOA, PIO_PA10A_RXD0 | PIO_PA11A_TXD0, PIO_PULLUP);

    sysclk_enable_peripheral_clock(ID_USART0);
    sam_usart_opt_t usart_opts = {
        .baudrate = 74880/*115200*/,
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

uint8_t esp_module_wifi_connect(const char* ssid, const char* password) {
    uint16_t esp_connect_retries = 100;

    while (esp_connect_retries >= 0) {
        uint8_t ssid_length = strlen(ssid);
        uint8_t password_length = strlen(password);

        sprintf(default_lcd->_lcd_string, "                ");
        sprintf(default_lcd->_lcd_string, "Conn WiFi %i", esp_connect_retries);
        lcd_write_lcd_string(default_lcd);
        delay_ms(100);
        /*sprintf(default_lcd->_lcd_string + 16, "                ");
        sprintf(default_lcd->_lcd_string + 16, "SSID length %i", ssid_length);
        lcd_write_lcd_string(default_lcd);
        delay_ms(1000);
        sprintf(default_lcd->_lcd_string + 16, "                ");
        sprintf(default_lcd->_lcd_string + 16, "Pass length %i", password_length);
        lcd_write_lcd_string(default_lcd);
        delay_ms(1000);
        sprintf(default_lcd->_lcd_string + 16, "                ");
        sprintf(default_lcd->_lcd_string + 16, "SSID:%s", ssid);
        lcd_write_lcd_string(default_lcd);
        delay_ms(1000);
        sprintf(default_lcd->_lcd_string + 16, "                ");
        sprintf(default_lcd->_lcd_string + 16, "Pass:%s", password);
        lcd_write_lcd_string(default_lcd);
        delay_ms(1000);*/

        esp_module_clear_status();
        //esp_module_write('c');
            default_lcd->lcd_lower[0] = 'c';
            lcd_write_lcd_string(default_lcd);
            //delay_ms(100);


        esp_module_write(ssid_length);
            default_lcd->lcd_lower[1] = ssid_length;
            lcd_write_lcd_string(default_lcd);
            //delay_ms(100);
        //esp_module_write(password_length);
            default_lcd->lcd_lower[2] = password_length;
            lcd_write_lcd_string(default_lcd);
            //delay_ms(100);

        for (uint8_t i = 0; i < ssid_length; i++) {
            esp_module_write(ssid[i]);
        }
        esp_module_write(0);

        for (uint8_t i = 0; i < password_length; i++) {
            esp_module_write(password[i]);
        }
        esp_module_write(0);


        int i = 0;
        do {
            if (esp_module_read_wait_timeout(1000)) {
                default_lcd->lcd_lower[i] = esp_rx_data;
                lcd_write_lcd_string(default_lcd);
                i++;
            }


            if (i >= 15) {
                delay_ms(1000);
                i = 0;
            }
        } while (1);


        //delay_ms(50);

        /*if (esp_module_read_wait_timeout(10000)) {
            sprintf(default_lcd->_lcd_string, "                ");
            sprintf(default_lcd->_lcd_string, "WiFi state:");
            sprintf(default_lcd->_lcd_string + 16, "                ");
            sprintf(default_lcd->_lcd_string + 16, "%i", esp_rx_data);
            lcd_write_lcd_string(default_lcd);
            delay_ms(100);

            return esp_rx_data;
            /*if (esp_rx_data == 0x01) {
                // Success
                return 0x01;
            } else {
                return 0x02;
            }*
        } else {
            sprintf(default_lcd->_lcd_string + 16, "                ");
            sprintf(default_lcd->_lcd_string + 16, "No recv...");
            lcd_write_lcd_string(default_lcd);
            delay_ms(500);
            //return 0x00;
        }*/

        esp_connect_retries--;
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
    return esp_module_read_wait_timeout(10000);
}

bool esp_module_read_wait_timeout(const uint32_t retries_timeout) {
    uint32_t retries_left = retries_timeout;

    while (usart_getchar(USART0, &esp_rx_data) != 0) {
        retries_left--;

        if (retries_left <= 0) {
            return false;
        }
    }

    return true;
}

void esp_module_write(uint8_t value) {
    usart_putchar(USART0, value);
}

void esp_module_clear_status() {
    esp_module_clear_queue();
    usart_reset_status(USART0);
}

void esp_module_clear_queue() {
    esp_module_read_wait_timeout(10000);
}

#ifdef __cplusplus
}
#endif
