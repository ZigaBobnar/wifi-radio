#include "asf.h"
#include "common.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "conf_dacc.h"

__EXTERN_C_BEGIN

#define DACC_ANALOG_CONTROL (DACC_ACR_IBCTLCH0(0x02) \
| DACC_ACR_IBCTLCH1(0x02) \
| DACC_ACR_IBCTLDACCORE(0x01))

#define MAX_DIGITAL	0x7ff
#define MAX_AMPLITUDE DACC_MAX_DATA

#define WAVE_SAMPLES 50

#define wave_to_dacc(wave, amplitude, max_digital, max_amplitude) \
(((int)(wave) * (amplitude) / (max_digital)) + (max_amplitude / 2))

uint32_t sample_index = 0;
uint32_t frequency = 0;
int32_t amplitude = 0;

const int16_t sine_wave_data[WAVE_SAMPLES] = {
	0x0,   0x100, 0x1fd, 0x2f1, 0x3da,
	0x4b3,  0x579,  0x629,  0x6c0,  0x73c,
	0x79b , 0x7db,  0x7fb,  0x7fb,  0x7db,
	0x79b,  0x73c,  0x6c0,  0x629,  0x579,
	0x4b3,  0x3da,  0x2f1,  0x1fd,  0x100,

	-0x0,    -0x100,  -0x1fd,  -0x2f1,  -0x3da,
	-0x4b3,  -0x579,  -0x629,  -0x6c0,  -0x73c,
	-0x79b,  -0x7db,  -0x7fb,  -0x7fb,  -0x7db,
	-0x79b,  -0x73c,  -0x6c0,  -0x629,  -0x579,
	-0x4b3,  -0x3da,  -0x2f1,  -0x1fd,  -0x100
};

int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	delay_init();
	ioport_init();

	/* Disable watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;

	sysclk_enable_peripheral_clock(DACC_ID);

	dacc_reset(DACC_BASE);

	dacc_set_transfer_mode(DACC_BASE, 0);

	dacc_set_power_save(DACC_BASE, 0, 0);

	dacc_set_timing(DACC_BASE, 0x08, 0, 0x10);

	dacc_set_channel_selection(DACC_BASE, DACC_CHANNEL);

	dacc_enable_channel(DACC_BASE, DACC_CHANNEL);

	dacc_set_analog_control(DACC_BASE, DACC_ANALOG_CONTROL);

	amplitude = MAX_AMPLITUDE / 2;
	frequency = 1000;

	SysTick_Config(sysclk_get_cpu_hz() / (frequency * WAVE_SAMPLES));

	while (1) {}
}

void SysTick_Handler(void) {
	uint32_t status, dac_value;

	status = dacc_get_interrupt_status(DACC_BASE);

	if ((status & DACC_ISR_TXRDY) == DACC_ISR_TXRDY) {
		sample_index++;

		if (sample_index >= WAVE_SAMPLES) {
			sample_index = 0;
		}

		dac_value = wave_to_dacc(sine_wave_data[sample_index], amplitude, MAX_DIGITAL * 2, MAX_AMPLITUDE);

		dacc_write_conversion_data(DACC_BASE, dac_value);
	}
}

__EXTERN_C_END
