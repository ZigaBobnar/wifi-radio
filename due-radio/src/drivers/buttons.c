#include "drivers/buttons.h"
#include "app/runtime.h"

__EXTERN_C_BEGIN

volatile buttons_runtime_t* g_buttons;

void buttons_init()
{
	g_buttons = runtime->buttons;

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
	uint8_t state =
		(!ioport_get_pin_level(PIO_PC26_IDX) << 3) |
		(!ioport_get_pin_level(PIO_PC25_IDX) << 2) |
		(!ioport_get_pin_level(PIO_PC24_IDX) << 1) |
		(!ioport_get_pin_level(PIO_PC23_IDX) << 0);

	uint8_t old_state = g_buttons->state;
	g_buttons->rising = ~old_state & state;
	g_buttons->falling = old_state & ~state;

	g_buttons->state = state;
}

bool button_pressed(int button) {
	return g_buttons->rising & (1 << button);
}

bool button_released(int button) {
	return g_buttons->falling & (1 << button);
}

bool button_state(int button) {
	return g_buttons->state & (1 << button);
}

__EXTERN_C_END
