#ifndef DRIVERS_BUTTONS_H_
#define DRIVERS_BUTTONS_H_

#include "due-radio/common.h"

/**
 * Buttons driver interface
 */

__EXTERN_C_BEGIN

void buttons_init(void);

void buttons_read(void);

bool button_pressed(int button);
bool button_released(int button);
bool button_state(int button);

__EXTERN_C_END

#endif  // DRIVERS_BUTTONS_H_
