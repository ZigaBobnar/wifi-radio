#include "audio_player.h"

#include <time.h>
#include "dac.h"

__EXTERN_C_BEGIN

uint8_t audio_sample_buff[AUDIO_PLAYER_SAMPLE_QUEUE_SIZE];
fifo_t audio_sample_fifo = {
    .read_idx = 0,
    .write_idx = 0,
    .size = AUDIO_PLAYER_SAMPLE_QUEUE_SIZE,
    .buffer = audio_sample_buff,
};

bool is_running = false;

void audio_player_init() {
	sysclk_enable_peripheral_clock(ID_TC0);
    tc_init(TC0, 0, TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC);
	tc_enable_interrupt(TC0, 0, TC_IER_CPCS);
}

void audio_player_start() {
	float T_ms = 1000.0 / AUDIO_PLAYER_SAMPLE_FREQUENCY;
	// tc_write_rc(TC0, 0, (uint32_t)(T_ms * CLOCKS_PER_SEC * 32));
	tc_write_rc(TC0, 0, (uint32_t)(T_ms * CLOCKS_PER_SEC * 25));

	NVIC_EnableIRQ(TC0_IRQn);
    tc_start(TC0, 0);
}

void audio_player_stop() {
	NVIC_DisableIRQ(TC0_IRQn);
	tc_stop(TC0, 0);
}

void TC0_Handler() {
	static uint32_t d = 255;

	dac_write(d);
	if (d == 255) {
		d = 0;
	} else {
		d = 255;
	}

	tc_get_status(TC0, 0);
}

__EXTERN_C_END
