#ifndef APP_UI_H_
#define APP_UI_H_

#include "due-radio/common.h"

/**
 * User interface processor for the project.
 */

typedef enum {
    UI_STATE_LOADING,
    UI_STATE_CLOCK,
    UI_STATE_PLAYING,
    UI_STATE_ERROR,
} ui_state;

__EXTERN_C_BEGIN

void ui_init(void);
void ui_run(void);
void ui_set_state(ui_state state);
ui_state ui_get_state(void);

void ui_process_state_clock(void);
void ui_process_state_playing(void);

void ui_lcd_cleanup(void);
void ui_lcd_display_current_time(bool force_update);
void ui_lcd_display_playback_info(bool force_update);

__EXTERN_C_END

#endif  // APP_UI_H_
