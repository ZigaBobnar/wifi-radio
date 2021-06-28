#ifndef DRIVERS_DAC_H_
#define DRIVERS_DAC_H_

#include "common.h"
#include <dacc.h>

/**
 * DAC (Digital to analog converter) interface
 *
 * This interface uses hardware DACC.
 */

#define DACC_ANALOG_CONTROL (DACC_ACR_IBCTLCH0(0x02) \
	| DACC_ACR_IBCTLCH1(0x02) \
	| DACC_ACR_IBCTLDACCORE(0x01))

__EXTERN_C_BEGIN

// typedef uint32_t (*ptr_get_next_sample_function)();

struct _dac {
	uint32_t channel;
	uint32_t max_value;
	uint32_t min_value;
	uint32_t sampling_frequency;
	// ptr_get_next_sample_function get_next_sample_function;
};
typedef struct _dac dac_t;

/**
 * Initializes the DACC hardware.
 */
void dac_init(dac_t* dac);

/**
 * Checks whether DACC is ready to accept data.
 */
bool dac_tx_ready(void);

/**
 * Writes a value to the DACC buffer.
 */
void dac_write(uint32_t value);

__EXTERN_C_END

#endif  // DRIVERS_DAC_H_
