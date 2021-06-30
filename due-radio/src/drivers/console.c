#include "drivers/console.h"
#include <stdarg.h>
#include "drivers/esp_module.h"

__EXTERN_C_BEGIN

uint8_t console_rx_fifo_buff[CONSOLE_RX_QUEUE_SIZE];
fifo_t console_rx_fifo = {
    .read_idx    = 0,
    .write_idx   = 0,
    .size        = CONSOLE_RX_QUEUE_SIZE,
    .buffer      = console_rx_fifo_buff,
};

int console_rx_line_ready = 0;

void console_init() {
    sysclk_enable_peripheral_clock(ID_PIOA);
    pio_set_peripheral(PIOA, PIO_PERIPH_A, PINS_UART);
    pio_pull_up(PIOA, PINS_UART, PIO_PULLUP);

    sysclk_enable_peripheral_clock(ID_UART);
    sam_uart_opt_t uart_opts = {
        .ul_mck = SystemCoreClock,
        .ul_baudrate = CONSOLE_BAUD,
        .ul_mode = UART_MR_CHMODE_NORMAL | UART_MR_PAR_NO,
    };

    uart_init(UART, &uart_opts);
}

void console_enable() {
    UART->UART_CR = UART_CR_RSTSTA | UART_CR_RSTTX | UART_CR_RSTRX;

    UART->UART_IER = UART_IER_RXRDY;
    NVIC_EnableIRQ(UART_IRQn);

	UART->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
}

/*
void console_process_input() {
    if (console_rx_line_ready > 0) {
        char* input_line = calloc(128, sizeof(char));
        int input_line_index = 0;
        char current_char = 0;

        do {
            fifo_read(&console_rx_fifo, &current_char, 1);

            if (current_char != '\n') {
                input_line[input_line_index++] = current_char;
            }

            if (input_line_index > 127) {
                console_put_line("Console> Error: Failed to read line. Input is too long.");
            }
        } while (current_char != '\n');

        console_rx_line_ready--;

        if (input_line == "esp stop_stream") {
            //esp_module_stop_stream();
        } else if (input_line == "esp start_stream") {
            //esp_module_start_stream();
        } else if (input_line == "esp wifi_connect") {
            esp_module_wifi_connect(WIFI_SSID, WIFI_PASSWORD);
        } else if (input_line == "restart") {
            console_put_line(CONSOLE_VT100_COLOR_TEXT_CYAN
                "Resetting..."
                CONSOLE_VT100_COLOR_TEXT_DEFAULT);
            delay_ms(50);

            __DSB;
            SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
            RSTC->RSTC_CR = RSTC_CR_KEY(0xA5) | RSTC_CR_PERRST | RSTC_CR_PROCRST;
            NVIC_SystemReset();
        }

        free(input_line);
    }
}
*/

void console_put_char(uint8_t value) {
    while (!(UART->UART_SR & UART_SR_TXRDY)) {}

    UART->UART_THR = UART_THR_TXCHR(value);
}

void console_put_raw_string(const char* str) {
    const char* c = str;

    while (*c != 0) {
        console_put_char(*c);
        c++;
    }
}

void console_put_line(const char* str) {
    // console_put_raw_string(CONSOLE_VT100_CURSOR_UP);
    // console_put_char(CONSOLE_ASCII_CARRIAGE_RETURN);
    console_put_raw_string(str);
    console_put_char('\n');
}

void console_put(const char* str) {
    console_put_raw_string(str);
}

void console_put_formatted(const char* format, ...) {
    va_list args;
    va_start(args, format);

    size_t required_size = vsnprintf(NULL, 0, format, args) + 1;

    char* buffer = calloc(required_size + 1, sizeof(char));

    vsnprintf(buffer, required_size, format, args);

    console_put_line(buffer);

    free(buffer);
}


bool console_char_ready() {
    return fifo_has_next_item(&console_rx_fifo);
}

void console_wait_until_char_ready() {
    while (!fifo_has_next_item(&console_rx_fifo)) {}
}

uint8_t console_get_char() {
    uint8_t buf;

    fifo_read(&console_rx_fifo, &buf, 1);

    return buf;
}

uint8_t console_peek_char() {
    uint8_t buf;

    fifo_peek(&console_rx_fifo, &buf, 1);

    return buf;
}

void UART_Handler() {
    // Handle RX situations by putting received bytes into fifo.
    if (UART->UART_IMR & UART_IMR_RXRDY) {
        uint8_t value = UART->UART_RHR;

        // TODO:
        //console_put_char(value); // Console echo
        fifo_write_single(&console_rx_fifo, value);
        if (value == '\n') {
            console_rx_line_ready++;
        }
    }
}

__EXTERN_C_END
