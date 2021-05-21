#include "console.h"

__EXTERN_C_BEGIN

console_t* active_console = NULL;

uint8_t console_rx_fifo_buff[CONSOLE_RX_QUEUE_SIZE];
fifo_t console_rx_fifo = {
    .read_idx    = 0,
    .write_idx   = 0,
    .size        = CONSOLE_RX_QUEUE_SIZE,
    .buffer      = console_rx_fifo_buff,
};

void console_init(console_t* console) {
    sysclk_enable_peripheral_clock(ID_PIOA);
    pio_set_peripheral(PIOA, PIO_PERIPH_A, PINS_UART);
    pio_pull_up(PIOA, PINS_UART, PIO_PULLUP);

    sysclk_enable_peripheral_clock(ID_UART);
    sam_uart_opt_t uart_opts = {
        .ul_mck = SystemCoreClock,
        .ul_baudrate = console->baud,
        .ul_mode = UART_MR_CHMODE_NORMAL | UART_MR_PAR_NO,
    };

    uart_init(UART, &uart_opts);

    active_console = console;
}

void console_enable() {
    UART->UART_CR = UART_CR_RSTSTA | UART_CR_RSTTX | UART_CR_RSTRX;

    UART->UART_IER = UART_IER_RXRDY;
    NVIC_EnableIRQ(UART_IRQn);

	UART->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
}


void console_put_char(uint8_t value) {
    while (!(UART->UART_SR & UART_SR_TXRDY)) {}

    UART->UART_THR = UART_THR_TXCHR(value);
}

void console_put_string(const char* str) {
    const char* c = str;

    while (*c != 0) {
        console_put_char(*c);
        c++;
    }
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
        fifo_write_single(&console_rx_fifo, UART->UART_RHR);
    }
}

__EXTERN_C_END
