#include "esp_module.h"
#include <string.h>
#include "console.h"
#include "timeguard.h"

__EXTERN_C_BEGIN

uint8_t esp_rx_data = 0;
lcd_t* default_lcd = NULL;

uint8_t esp_rx_fifo_buff[ESP_RX_QUEUE_SIZE];
fifo_t esp_rx_fifo = {
    .read_idx    = 0,
    .write_idx   = 0,
    .size        = ESP_RX_QUEUE_SIZE,
    .buffer      = esp_rx_fifo_buff,
};

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

    USART0->US_IER = US_IER_RXRDY;
    NVIC_EnableIRQ(USART0_IRQn);
}

bool esp_module_init() {
    int8_t retries_left = 10;

    while (retries_left >= 0) {
        esp_module_clear_status();
        esp_module_tx_put_char('s');
        char buff[64];
        sprintf(buff, "TX-ESP> Sending 's' command (retries left: %i)\n", retries_left);
        console_put_string(buff);

        if (esp_module_rx_read_wait()) {
            if (esp_rx_data == 'O' && esp_module_rx_read_wait()) {
                if (esp_rx_data == 'K') {
                    return true;
                }
            }
        }

        retries_left--;

        delay_ms(500);
    }

    return false;
}

uint8_t esp_module_wifi_connect(const char* ssid, const char* password) {
    return 0;
    /*int16_t esp_connect_retries = 10;

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
        delay_ms(1000);*

        esp_module_clear_status();
        //esp_module_write('c');
            default_lcd->lcd_lower[0] = 'c';
            lcd_write_lcd_string(default_lcd);
            //delay_ms(100);


        esp_module_tx_put_char(ssid_length);
            default_lcd->lcd_lower[1] = ssid_length;
            lcd_write_lcd_string(default_lcd);
            //delay_ms(100);
        //esp_module_write(password_length);
            default_lcd->lcd_lower[2] = password_length;
            lcd_write_lcd_string(default_lcd);
            //delay_ms(100);

        for (uint8_t i = 0; i < ssid_length; i++) {
            esp_module_tx_put_char(ssid[i]);
        }
        esp_module_tx_put_char(0);

        for (uint8_t i = 0; i < password_length; i++) {
            esp_module_tx_put_char(password[i]);
        }
        esp_module_tx_put_char(0);


        int i = 0;
        do {
            if (esp_module_rx_read_wait_timeout(1000)) {
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
        }*

        esp_connect_retries--;
    }*/
}

void esp_module_start_stream() {
    esp_module_clear_status();
    esp_module_tx_put_char('S');
}

void esp_module_stop_stream() {
    esp_module_clear_status();
    esp_module_tx_put_char('E');
    esp_module_clear_status();
}

void esp_module_tx_put_char(uint8_t value) {
	while (!(USART0->US_CSR & US_CSR_TXRDY)) {}

	USART0->US_THR = US_THR_TXCHR(value);
}

bool esp_module_rx_read() {
    return (fifo_read(&esp_rx_fifo, &esp_rx_data, 1) == 1);
}

bool esp_module_rx_read_wait() {
    return esp_module_rx_read_wait_timeout(500);
}

bool esp_module_rx_read_wait_timeout(const int32_t timeout_ms) {
    int32_t read_start_ms = timeguard_get_time_ms();

    while (!esp_module_rx_char_ready()) {
        if (timeguard_get_time_ms() - read_start_ms > timeout_ms) {
            console_put_string("E: ESP module rx wait timed out after 1000 ms\n");
            return false;
        }
    }

    return esp_module_rx_read();
}

bool esp_module_rx_char_ready() {
    return fifo_has_next_item(&esp_rx_fifo);
}

void esp_module_rx_wait_until_char_ready() {
    while (!fifo_has_next_item(&esp_rx_fifo)) {}
}

void esp_module_clear_status() {
    esp_module_rx_clear_queue();
    usart_reset_status(USART0);
}

void esp_module_rx_clear_queue() {
    fifo_discard(&esp_rx_fifo);
}

uint8_t esp_module_rx_get_char() {
    uint8_t buf;

    fifo_read(&esp_rx_fifo, &buf, 1);

    return buf;
}

uint8_t esp_module_rx_peek_char() {
    uint8_t buf;

    fifo_peek(&esp_rx_fifo, &buf, 1);

    return buf;
}

void USART0_Handler() {
    static char line_buff[255];
    static int line_idx = 0;

    // Handle RX situations by putting received bytes into fifo.
    if (USART0->US_IMR & US_IMR_RXRDY) {
        uint8_t recv = USART0->US_RHR;
        fifo_write_single(&esp_rx_fifo, recv);

        *(line_buff + line_idx) = recv;
        line_idx++;
        if (recv == '\n' || line_idx >= 250) {

            console_put_string("ESP> ");
            console_put_string(line_buff);
            if (recv != '\n') {
                console_put_char('\n');
            }

            line_idx = 0;
            memset(line_buff, 0, sizeof(line_buff));
        }
    }
}

__EXTERN_C_END
