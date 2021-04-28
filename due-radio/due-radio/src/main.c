#include "asf.h"
#include "common.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"

__EXTERN_C_BEGIN

int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	delay_init();
	ioport_init();

	/* Disable watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;

	while (1) {}
}

__EXTERN_C_END
