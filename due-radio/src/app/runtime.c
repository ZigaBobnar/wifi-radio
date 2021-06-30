#include "app/runtime.h"

//
// LCD
//
static char lcd_buffer[33] = "                                ";
static lcd_t lcd_instance = {
    .rs = LCD_RS_PIN,
    .rw = LCD_RW_PIN,
    .enable = LCD_ENABLE_PIN,
    .d4 = LCD_D4_PIN,
    .d5 = LCD_D5_PIN,
    .d6 = LCD_D6_PIN,
    .d7 = LCD_D7_PIN,

    ._lcd_string = lcd_buffer,
    .lcd_upper = lcd_buffer,
    .lcd_lower = lcd_buffer + 16,
};

//
// DAC
//
static dac_t dac_instance = {
	.channel = DAC_CHANNEL,
};

//
// Audio player
//
static uint8_t audio_player_sample_buffer[AUDIO_PLAYER_SAMPLE_BUFFER_SIZE];
static fifo_t audio_player_sample_buffer_fifo = {
    .read_idx = 0,
    .write_idx = 0,
    .size = AUDIO_PLAYER_SAMPLE_BUFFER_SIZE,
    .buffer = audio_player_sample_buffer,
};
static audio_player_runtime_t audio_player_runtime_instance = {
    .is_running = false,
    .is_buffering = false,
    .buffered_samples = 0,
    .buffer_fifo = &audio_player_sample_buffer_fifo,
};

//
// Buttons
//
static buttons_runtime_t buttons_runtime_instance = {
    .state = 0,
    .rising = 0,
    .falling = 0,
};

//
// UI
//
static ui_runtime_t ui_runtime_instance = {
    .state = UI_STATE_LOADING,
    .state_switched_dirty = true,
};

//
// Runtime
//
static app_runtime_t runtime_instance = {
    .lcd = &lcd_instance,
    .dac = &dac_instance,
    .player = &audio_player_runtime_instance,
    .buttons = &buttons_runtime_instance,
    .ui = &ui_runtime_instance,
};

__EXTERN_C_BEGIN

app_runtime_t* runtime = &runtime_instance;

__EXTERN_C_END
