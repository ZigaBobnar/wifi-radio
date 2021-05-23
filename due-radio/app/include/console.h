#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "common.h"
#include "console_definitions.h"
#include "fifo.h"

/**
 * Serial console interface
 *
 * This is an interface for UART communication on Arduino Due pins RX0 and TX0.
 * Those pins are also wired to the serial to USB chip on programming USB port
 * so this console can be very easily used as interface to computer to see the
 * controller status and send commands.
 */

//#ifndef CONSOLE_BAUD
#define CONSOLE_BAUD CONSOLE_BAUD_115200
//#endif  // CONSOLE_BAUD

#ifndef CONSOLE_RX_QUEUE_SIZE
#define CONSOLE_RX_QUEUE_SIZE 128
#endif  // CONSOLE_RX_QUEUE_SIZE

__EXTERN_C_BEGIN

extern uint8_t console_rx_fifo_buff[CONSOLE_RX_QUEUE_SIZE];
extern fifo_t console_rx_fifo;

/**
 * Initializes the console by enabling the proper ioport, configuring the
 * clock and initializing the UART hardware.
 */
void console_init(void);

/**
 * Enable the UART receiving and transmitting hardware.
 */
void console_enable(void);


/**
 * Processes the input received through console.
 *
 * This checks if a valid command has been provided and executes it.
 */
void console_process_input(void);


/**
 * Write (transmit) single character to the TX line.
 */
void console_put_char(const uint8_t value);

/**
 * Write (transmit) null-terminated string value directly to the TX line.
 */
void console_put_raw_string(const char* str);

/**
 * Write (transmit) null-terminated string value to the TX line and add LF
 * character to the end.
 */
void console_put_line(const char* str);

/**
 * Write (transmit) null-terminated string value to the TX line. This will not
 * add LF character to the end.
 */
void console_put(const char* str);

/**
 * Write (transmit) formatted string using sprintf and print it as line.
 */
void console_put_formatted(const char* format, ...)
    __attribute__((format (__printf__, 1, 0)));


/**
 * Checks whether at least single character is available in receive buffer.
 */
bool console_char_ready(void);

/**
 * Waits until at least one character is available in receive buffer.
 */
void console_wait_until_char_ready(void);

/**
 * Reads the next character from buffer.
 */
uint8_t console_get_char(void);

/**
 * Peeks (reads without advancing the buffer) the next character from buffer.
 */
uint8_t console_peek_char(void);

__EXTERN_C_END

#endif  // CONSOLE_H_
