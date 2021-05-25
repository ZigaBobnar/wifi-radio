#include "esp_module.h"
#include <string.h>
#include <stdarg.h>
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

bool stream_running = false;

void esp_module_hardware_setup(lcd_t* lcd_ptr) {
    default_lcd = lcd_ptr;

    pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA10A_RXD0 | PIO_PA11A_TXD0);
    pio_pull_up(PIOA, PIO_PA10A_RXD0 | PIO_PA11A_TXD0, PIO_PULLUP);

    sysclk_enable_peripheral_clock(ID_USART0);
    sam_usart_opt_t usart_opts = {
        .baudrate = /*74880*//*115200*/500000,
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
        console_put_formatted("ESP> Sending module init command, tries left: %i", retries_left);

        esp_module_clear_status();

        esp_module_tx_put_line("status");

        char* response = esp_module_rx_read_line(31, 100);
        if (response != NULL) {
            if (strcmp(response, "OK") == 0) {
                console_put_line("ESP> Module initialized.");
                free(response);

                return true;
            }
            
            console_put_line("ESP> Module init failed. Incorrect response.");
            free(response);
        }

        retries_left--;

        delay_ms(500);
    }

    return false;
}

bool esp_module_wifi_connect(const char* ssid, const char* password) {
    console_put_line("ESP> Sending wifi connect command");

    esp_module_clear_status();

    esp_module_tx_put_formatted("connect %s %s", ssid, password);
    delay_ms(100);

    char* response = esp_module_rx_read_line(127, 15000);
    if (response != NULL) {
        if (strcmp(response, "OK") == 0) {
            console_put_line("ESP> Module initialized.");
            free(response);

            return true;
        }
        
        console_put_line("ESP> Module init failed. Incorrect response.");
        free(response);

        return false;
    }
}

void esp_module_start_stream() {
    console_put_line("ESP> Sending start stream command");

    esp_module_clear_status();
    esp_module_tx_put_line("start_stream");
    stream_running = true;

    delay_ms(100);
    esp_module_clear_status();
}

void esp_module_stop_stream() {
    console_put_line("ESP> Sending stop stream command");

    esp_module_clear_status();
    esp_module_tx_put_line("stop_stream");
    stream_running = false;

    delay_ms(100);
    esp_module_clear_status();
}

void esp_module_tx_put_char(uint8_t value) {
	while (!(USART0->US_CSR & US_CSR_TXRDY)) {}

	USART0->US_THR = US_THR_TXCHR(value);
}

void esp_module_tx_put_line(const char* value) {
    console_put_formatted("ESP(TX)> %s", value);

    for (int i = 0; i < strlen(value); i++) {
        esp_module_tx_put_char(value[i]);
    }
    esp_module_tx_put_char('\n');
}

void esp_module_tx_put_formatted(const char* format, ...) {
    va_list args;
    va_start(args, format);

    size_t required_size = vsnprintf(NULL, 0, format, args) + 1;

    char* buffer = calloc(required_size + 1, sizeof(char));

    vsnprintf(buffer, required_size, format, args);

    esp_module_tx_put_line(buffer);

    free(buffer);
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
            console_put_formatted("ESP> Error: ESP module rx wait timed out after %i ms", (int)timeout_ms);
            return false;
        }
    }

    return esp_module_rx_read();
}

char* esp_module_rx_read_line(size_t max_length, int32_t timeout_ms) {
    char* response = calloc(max_length + 1, sizeof(char));
    int response_index = 0;

    do {
        if (!esp_module_rx_read_wait_timeout(timeout_ms)) {
            console_put_line("ESP> Error: Failed to read line. Did not receive a character in proper time interval.");

            free(response);
            return NULL;
        }

        if (esp_rx_data != '\n') {
            response[response_index++] = esp_rx_data;
        } else {
            return response;
        }

        if (response_index > max_length) {
            console_put_line("ESP> Error: Failed to read line. Response is too long.");
            
            free(response);
            return NULL;
        }
    } while (1);

    console_put_line("ESP> Error: Failed to read line. Unknown error.");

    free(response);
    return NULL;
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

        if (!stream_running) {
            if (recv != '\n') {
                *(line_buff + line_idx) = recv;
                line_idx++;
            }

            if (recv == '\n' || line_idx >= 250) {
                console_put_formatted("ESP(RX)> %s", line_buff);

                line_idx = 0;
                memset(line_buff, 0, sizeof(line_buff));
            }
        }
    }
}

__EXTERN_C_END
