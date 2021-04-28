#ifndef DRIVERS_DAC_H_
#define DRIVERS_DAC_H_

#include "../common.h"
#include <dacc.h>

__EXTERN_C_BEGIN

#define DACC_ANALOG_CONTROL (DACC_ACR_IBCTLCH0(0x02) \
| DACC_ACR_IBCTLCH1(0x02) \
| DACC_ACR_IBCTLDACCORE(0x01))

typedef uint32_t (*ptr_get_next_sample_function)();

struct _dac {
	uint32_t channel;
	uint32_t max_value;
	uint32_t min_value;
	uint32_t sampling_frequency;
	ptr_get_next_sample_function get_next_sample_function;
};
typedef struct _dac dac_t;

dac_t* active_dac;

void dac_init(dac_t* dac);
void dac_activate(dac_t* dac);

__EXTERN_C_END

#endif  // DRIVERS_DAC_H_
