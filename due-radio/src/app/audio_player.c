#include "app/audio_player.h"

#include <time.h>
#include "app/runtime.h"
#include "drivers/dac.h"
#include "drivers/esp_module.h"
#include "drivers/console.h"
#include "utils/timeguard.h"

__EXTERN_C_BEGIN

void audio_player_init() {
	fifo_discard(runtime->player->buffer_fifo);
	runtime->player->buffered_samples = 0;
	runtime->player->is_buffering = false;
	runtime->player->is_running = false;

	sysclk_enable_peripheral_clock(ID_TC0);
    tc_init(TC0, 0, TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC);
	tc_enable_interrupt(TC0, 0, TC_IER_CPCS);
}

void audio_player_start() {
	console_put_formatted("AudioPlayer> Starting, frequency: %i", AUDIO_PLAYER_SAMPLE_FREQUENCY);

	runtime->player->is_running = true;

	float T_ms = 1000.0 / AUDIO_PLAYER_SAMPLE_FREQUENCY;
	// tc_write_rc(TC0, 0, (uint32_t)(T_ms * CLOCKS_PER_SEC * 32));
	tc_write_rc(TC0, 0, (uint32_t)(T_ms * CLOCKS_PER_SEC * 25));

	NVIC_EnableIRQ(TC0_IRQn);
    tc_start(TC0, 0);
}

void audio_player_stop() {
	console_put_formatted("AudioPlayer> Stopping");

	runtime->player->is_running = false;

	NVIC_DisableIRQ(TC0_IRQn);
	tc_stop(TC0, 0);
}

void TC0_Handler() {
	if (runtime->player->buffered_samples > 0) {
		uint8_t sample_value = 0;
		fifo_read(runtime->player->buffer_fifo, &sample_value, 1);

		dac_write(sample_value);

		runtime->player->buffered_samples--;
	}

	tc_get_status(TC0, 0);
}

void audio_player_ensure_buffered(void) {
	static int32_t last_refill = 0;

	int32_t current_time_ms = timeguard_get_time_ms();

	if (runtime->player->is_running &&
			current_time_ms - last_refill > AUDIO_PLAYER_MIN_REBUFFER_TIME_THRESHOLD &&
			runtime->player->buffered_samples < AUDIO_PLAYER_REBUFFER_MAX_SAMPLES_THRESHOLD) {
		last_refill = current_time_ms;

        audio_player_fill_buffer();
    }
}

void audio_player_fill_buffer() {
	console_put_formatted("AudioPlayer> Filling buffer...");

	esp_module_get_next_chunk();
}

__EXTERN_C_END
