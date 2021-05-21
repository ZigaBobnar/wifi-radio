#ifndef CONSOLE_H_
#define CONSOLE_H_

#ifndef CONSOLE_RX_QUEUE_SIZE
#define CONSOLE_RX_QUEUE_SIZE 128
#endif  // CONSOLE_RX_QUEUE_SIZE

#define CONSOLE_BAUD_74880  (74880)
#define CONSOLE_BAUD_115200 (115200)

#include "asf.h"
#include "fifo.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _console {
    uint32_t baud;
};
typedef struct _console console_t;

extern console_t* active_console;

extern uint8_t console_rx_fifo_buff[CONSOLE_RX_QUEUE_SIZE];
extern fifo_t console_rx_fifo;


void console_init(console_t* console);
void console_enable(void);

void console_put_char(const uint8_t value);
void console_put_string(const char* str);

bool console_char_ready(void);
void console_wait_until_char_ready(void);
uint8_t console_get_char(void);
uint8_t console_peek_char(void);

#ifdef __cplusplus
}
#endif

#endif  // CONSOLE_H_
