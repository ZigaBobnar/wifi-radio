#ifndef UI_H_
#define UI_H_

#include "common.h"

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

void ui_update_current_time(void);
int32_t ui_get_current_time_ms(void);
void ui_set_current_time_ms(int32_t current_time_ms);
void ui_set_current_date(int32_t year, int32_t month, int32_t day);

void ui_lcd_display_current_time(bool force_update);
void ui_lcd_display_playback_info(bool force_update);

__EXTERN_C_END

#endif  // UI_H_
