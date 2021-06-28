#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "common.h"

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

#endif  // BUTTONS_H_
