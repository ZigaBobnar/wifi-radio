#include "buttons.h"

__EXTERN_C_BEGIN

uint32_t g_buttons_state = 0;
uint32_t g_buttons_rising = 0;
uint32_t g_buttons_falling = 0;

void buttons_init()
{
	ioport_init();

	ioport_enable_pin(PIO_PC26_IDX);
	ioport_enable_pin(PIO_PC25_IDX);
	ioport_enable_pin(PIO_PC24_IDX);
	ioport_enable_pin(PIO_PC23_IDX);

	ioport_set_pin_dir(PIO_PC26_IDX, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIO_PC25_IDX, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIO_PC24_IDX, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIO_PC23_IDX, IOPORT_DIR_INPUT);
}

void buttons_read()
{
	uint32_t state =
		(!ioport_get_pin_level(PIO_PC26_IDX) << 3) |
		(!ioport_get_pin_level(PIO_PC25_IDX) << 2) |
		(!ioport_get_pin_level(PIO_PC24_IDX) << 1) |
		(!ioport_get_pin_level(PIO_PC23_IDX) << 0);

	g_buttons_rising = ~g_buttons_state & state;
	g_buttons_falling = g_buttons_state & ~state;

	g_buttons_state = state;
}

bool button_pressed(int button) {
	return g_buttons_rising & (1 << button);
}

bool button_released(int button) {
	return g_buttons_falling & (1 << button);
}

bool button_state(int button) {
	return g_buttons_state & (1 << button);
}
__EXTERN_C_END
