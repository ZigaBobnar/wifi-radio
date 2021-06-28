#ifndef SETUP_H_
#define SETUP_H_

#include "common.h"

__EXTERN_C_BEGIN

bool setup_begin(void);

bool setup_hardware(void);
bool setup_lcd(void);
bool setup_console(void);
bool setup_dac(void);
bool setup_esp_module(void);
bool setup_esp_wifi(void);
bool setup_timesync(void);
bool setup_player(void);

__EXTERN_C_END

#endif  // SETUP_H_
