#include "due-radio/drivers/dac.h"

__EXTERN_C_BEGIN

volatile dac_t* g_dac;

void dac_init(dac_t* dac) {
	g_dac = dac;

#if REAL_HARDWARE
	//sysclk_enable_peripheral_clock(ID_TC0);
	sysclk_enable_peripheral_clock(ID_DACC);

	dacc_reset(DACC);
	dacc_set_transfer_mode(DACC, DACC_MR_WORD_HALF);
	dacc_set_power_save(DACC, 0x00, 0x00);
	dacc_set_timing(DACC, 0x08, 0x00, 0x10);
	dacc_set_channel_selection(DACC, g_dac->channel);
	dacc_enable_channel(DACC, g_dac->channel);
	dacc_set_analog_control(DACC, DACC_ANALOG_CONTROL);
#endif
}

bool dac_tx_ready() {
#if REAL_HARDWARE
	uint32_t dacc_status = dacc_get_interrupt_status(DACC);

	return (dacc_status & DACC_ISR_TXRDY) == DACC_ISR_TXRDY;
#else
	return true;
#endif
}

void dac_write(uint32_t value) {
#if REAL_HARDWARE
	dacc_write_conversion_data(DACC, ((uint32_t)value));
#else
	(void)value;
#endif
}

/*
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
*/

__EXTERN_C_END
