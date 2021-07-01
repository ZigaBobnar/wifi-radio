#ifndef APP_RUNTIME_H_
#define APP_RUNTIME_H_

#include "common.h"
#include "drivers/lcd.h"
#include "drivers/dac.h"
#include "app/ui.h"
#include "app/audio_player.h"
#include "utils/fifo.h"

__EXTERN_C_BEGIN

typedef struct _audio_player_runtime {
    volatile bool is_running;
    volatile bool is_buffering;
    volatile int32_t buffered_samples;
    fifo_t* buffer_fifo;
} audio_player_runtime_t;

typedef struct _buttons_runtime {
    volatile uint8_t state;
    volatile uint8_t rising;
    volatile uint8_t falling;
} buttons_runtime_t;

typedef struct _ui_runtime {
    volatile ui_state state;
    volatile bool state_switched_dirty;
} ui_runtime_t;

typedef struct _clock_runtime {
    volatile uint16_t year;
    volatile uint8_t month;
    volatile uint8_t day;
    volatile uint8_t hour;
    volatile uint8_t minute;
    volatile uint8_t second;
    volatile bool clock_dirty;
} clock_runtime_t;

typedef struct _app_runtime {
    lcd_t* lcd;
    dac_t* dac;
    audio_player_runtime_t* player;
    buttons_runtime_t* buttons;
    ui_runtime_t* ui;
    clock_runtime_t* clock;
} app_runtime_t;

extern app_runtime_t* runtime;

__EXTERN_C_END

#endif  // APP_RUNTIME_H_
