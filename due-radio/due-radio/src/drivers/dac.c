#include "dac.h"
#include <asf.h>

__EXTERN_C_BEGIN

void dac_init(dac_t* dac) {
	sysclk_enable_peripheral_clock(ID_DACC);

	dacc_reset(DACC);
	dacc_set_transfer_mode(DACC, 0);
	dacc_set_power_save(DACC, 0, 0);
	dacc_set_timing(DACC, 0x08, 0, 0x10);
	dacc_set_channel_selection(DACC, dac->channel);
	dacc_enable_channel(DACC, dac->channel);
	dacc_set_analog_control(DACC, DACC_ANALOG_CONTROL);
}

void dac_activate(dac_t* dac) {
	SysTick_Config(sysclk_get_cpu_hz() / (dac->sampling_frequency));

	active_dac = dac;
}

void SysTick_Handler(void) {
	uint32_t status, dac_value;

	if (active_dac != NULL) {
		status = dacc_get_interrupt_status(DACC);

		if ((status & DACC_ISR_TXRDY) == DACC_ISR_TXRDY) {
			dac_value = active_dac->get_next_sample_function();

			dacc_write_conversion_data(DACC, dac_value);
		}
	}
}

__EXTERN_C_END
