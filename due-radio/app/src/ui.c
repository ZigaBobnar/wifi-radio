#include "ui.h"
#include "buttons.h"
#include "lcd.h"
#include "timeguard.h"
#include "audio_player.h"

__EXTERN_C_BEGIN

ui_state g_state;
bool g_state_dirty = true;
int32_t g_current_time_ms;
int32_t g_year, g_month, g_day;

void ui_init() {
    ui_set_state(UI_STATE_LOADING);
    buttons_init();
}

void ui_run() {
    ui_update_current_time();
    buttons_read();

    if (g_state == UI_STATE_LOADING) {
        // UI frozen
    } else if (g_state == UI_STATE_CLOCK) {
        // UI updating clock in correct intervals and handling start playback
        // presses

        if (g_state_dirty) {
            lcd_clear_upper();
            lcd_clear_lower();
            lcd_write_lcd_string();
        }

        ui_lcd_display_current_time(g_state_dirty);

        g_state_dirty = false;

        if (button_released(0)) {
            ui_set_state(UI_STATE_PLAYING);
        }
    } else if (g_state == UI_STATE_PLAYING) {
        // UI displaying playback info and handling playback commands

        if (g_state_dirty) {
            lcd_clear_upper();
            lcd_clear_lower();
            lcd_write_lcd_string();

            audio_player_start();
        }

        ui_lcd_display_playback_info(g_state_dirty);

        g_state_dirty = false;

        if (button_released(0)) {
            audio_player_stop();

            ui_set_state(UI_STATE_CLOCK);
        }
    } else if (g_state == UI_STATE_ERROR) {
        // UI frozen
    } else {

    }

    // Reset the system if buttons 1 and 3 are pressed for more than 3 seconds.
	if (button_released(0) && button_state(1) & 1 && button_state(2) & 1)
	{
        __DSB;
        SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
        RSTC->RSTC_CR = RSTC_CR_KEY(0xA5) | RSTC_CR_PERRST | RSTC_CR_PROCRST;
        NVIC_SystemReset();
	}
}

void ui_set_state(ui_state state) {
    g_state = state;
    g_state_dirty = true;
}

ui_state ui_get_state() {
    return g_state;
}

void ui_update_current_time() {
    static int32_t last_timeguard_time = 0;

    int32_t new_timeguard_time = timeguard_get_time_ms();

    g_current_time_ms += (new_timeguard_time - last_timeguard_time);

    if (g_current_time_ms >= 1000 * 60 * 60 * 24) {
        g_current_time_ms = 0;
    }

    last_timeguard_time = new_timeguard_time;
}

int32_t ui_get_current_time_ms() {
    return g_current_time_ms;
}

void ui_set_current_time_ms(int32_t current_time_ms) {
    g_current_time_ms = current_time_ms;
}

void ui_set_current_date(int32_t year, int32_t month, int32_t day) {
    g_year = year;
    g_month = month;
    g_day = day;
}

void ui_lcd_display_current_time(bool force_update) {
    static int32_t last_time = 0;
    static bool showing_date = false;
    static int32_t last_date_switch = 0;

    int32_t current_time = ui_get_current_time_ms();

    if (force_update) {
        last_time = current_time;
        last_date_switch = current_time;
        showing_date = false;
    }

    if (force_update || current_time - last_time > 150) {
        int hours = current_time / (1000 * 60 * 60);
        int minutes = current_time / (1000 * 60) - hours * 60;
        int seconds = current_time / 1000 - hours * 60 * 60 - minutes * 60;

        if (seconds % 2) {
            lcd_write_upper_formatted("    %02i:%02i:%02i    ", hours, minutes, seconds);
        } else {
            lcd_write_upper_formatted("    %02i %02i %02i    ", hours, minutes, seconds);
        }

        if (!force_update && current_time - last_date_switch > 5000) {
            showing_date = !showing_date;
            last_date_switch = current_time;
        }

        if (!showing_date) {
            lcd_write_lower_formatted("Press |> to play");
        } else {
            lcd_write_lower_formatted("   %02i.%02i.%i   ", g_day, g_month, g_year);
        }

        last_time = current_time;
    }
}

void ui_lcd_display_playback_info(bool force_update) {
    static int32_t last_switch_time = 0;
    static bool showing_clock = false;

    int32_t current_time = ui_get_current_time_ms();

    if (force_update) {
        last_switch_time = current_time;
        showing_clock = false;
    }

    if (current_time - last_switch_time > 5000) {
        last_switch_time = current_time;
        showing_clock = !showing_clock;
        force_update = true;
    }

    if (force_update || current_time - last_switch_time > 150) {
        if (!showing_clock) {
            lcd_write_upper_formatted("Playing...      ");
            lcd_write_lower_formatted("Press || to stop");
        } else {
            int hours = current_time / (1000 * 60 * 60);
            int minutes = current_time / (1000 * 60) - hours * 60;
            int seconds = current_time / 1000 - hours * 60 * 60 - minutes * 60;

            if (seconds % 2) {
                lcd_write_upper_formatted("    %02i:%02i:%02i    ", hours, minutes, seconds);
            } else {
                lcd_write_upper_formatted("    %02i %02i %02i    ", hours, minutes, seconds);
            }
        }
    }
}

__EXTERN_C_END
