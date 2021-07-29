#include "app/ui.h"
#include "runtime.h"
#include "drivers/buttons.h"
#include "drivers/lcd.h"
#include "drivers/clock.h"
#include "utils/timeguard.h"
#include "utils/system.h"
#include "app/audio_player.h"

__EXTERN_C_BEGIN

void ui_init() {
    ui_set_state(UI_STATE_LOADING);
    buttons_init();
}

void ui_run() {
	// TODO
    // ui_update_current_time();
    buttons_read();

    ui_state state = ui_get_state();
    if (state == UI_STATE_LOADING) {
        // UI frozen
    } else if (state == UI_STATE_CLOCK) {
        // UI updating clock in correct intervals and handling start playback
        // presses
        ui_process_state_clock();
    } else if (state == UI_STATE_PLAYING) {
        // UI displaying playback info and handling playback commands
        ui_process_state_playing();
    } else if (state == UI_STATE_ERROR) {
        // UI must stay frozen
    } else {
        // Unknown UI state
    }

    // Reset the system if button 1 is released while buttons 2 and 3 are pressed.
	if (button_released(0) && button_state(1) & 1 && button_state(2) & 1)
	{
        system_restart();
	}
}

void ui_set_state(ui_state state) {
    runtime->ui->state = state;
    runtime->ui->state_switched_dirty = true;
}

ui_state ui_get_state() {
    return runtime->ui->state;
}


void ui_process_state_clock() {
    if (runtime->ui->state_switched_dirty) {
        ui_lcd_cleanup();
    }

    ui_lcd_display_current_time(runtime->ui->state_switched_dirty);

    runtime->ui->state_switched_dirty = false;

    if (button_released(0)) {
        ui_set_state(UI_STATE_PLAYING);
    }
}

void ui_process_state_playing() {
    if (runtime->ui->state_switched_dirty) {
        ui_lcd_cleanup();

        audio_player_start();
    }

    ui_lcd_display_playback_info(runtime->ui->state_switched_dirty);

    runtime->ui->state_switched_dirty = false;

    if (button_released(0)) {
        audio_player_stop();

        ui_set_state(UI_STATE_CLOCK);
    }
}


void ui_lcd_cleanup() {
    lcd_clear_upper();
    lcd_clear_lower();
    lcd_write_lcd_string();
}

void ui_lcd_display_current_time(bool force_update) {
    static int32_t last_time = 0;
    static bool showing_date = false;
    static int32_t last_date_switch = 0;

    int32_t timeguard_time = timeguard_get_time_ms();
    if (force_update) {
        last_time = timeguard_time;
        last_date_switch = timeguard_time;
        showing_date = false;
    }

    if (force_update || timeguard_time - last_time > 150) {
        uint8_t hours, minutes, seconds;
        clock_get_time(&hours, &minutes, &seconds);

        if (seconds % 2) {
            lcd_write_upper_formatted("    %02i:%02i:%02i    ", hours, minutes, seconds);
        } else {
            lcd_write_upper_formatted("    %02i %02i %02i    ", hours, minutes, seconds);
        }

        if (!force_update && timeguard_time - last_date_switch > 5000) {
            showing_date = !showing_date;
            last_date_switch = timeguard_time;
        }

        //if (!showing_date) {
            lcd_write_lower_formatted("Press |> to play");
        /*} else {
            lcd_write_lower_formatted("   %02i.%02i.%i   ", g_day, g_month, g_year);
        }*/

        last_time = timeguard_time;
    }
}

void ui_lcd_display_playback_info(bool force_update) {
    static int32_t last_switch_time = 0;
    static bool showing_clock = false;

    int32_t timeguard_time = timeguard_get_time_ms();

    if (force_update) {
        last_switch_time = timeguard_time;
        showing_clock = false;
    }

    if (timeguard_time - last_switch_time > 5000) {
        last_switch_time = timeguard_time;
        showing_clock = !showing_clock;
        force_update = true;
    }

    if (force_update || timeguard_time - last_switch_time > 150) {
        if (!showing_clock) {
            lcd_write_upper_formatted("Playing...      ");
            lcd_write_lower_formatted("Press || to stop");
        } else {
            uint8_t hours, minutes, seconds;
            clock_get_time(&hours, &minutes, &seconds);

            if (seconds % 2) {
                lcd_write_upper_formatted("    %02i:%02i:%02i    ", hours, minutes, seconds);
            } else {
                lcd_write_upper_formatted("    %02i %02i %02i    ", hours, minutes, seconds);
            }
        }
    }
}

__EXTERN_C_END
