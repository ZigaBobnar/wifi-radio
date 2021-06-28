#include "audio_player.h"

#include <time.h>
#include "dac.h"
#include "esp_module.h"
#include "console.h"
#include "timeguard.h"

uint8_t audio_player_sample_buff[AUDIO_PLAYER_SAMPLE_QUEUE_SIZE];
fifo_t audio_player_sample_fifo = {
    .read_idx = 0,
    .write_idx = 0,
    .size = AUDIO_PLAYER_SAMPLE_QUEUE_SIZE,
    .buffer = audio_player_sample_buff,
};

__EXTERN_C_BEGIN

fifo_t* audio_player_buffer = NULL;
int32_t audio_player_buffered_samples = 0;
int32_t audio_player_buffering_samples_left = 0;
bool audio_player_buffering = false;
bool audio_player_running = false;

void audio_player_init() {
	audio_player_buffer = &audio_player_sample_fifo;
	audio_player_buffering_samples_left = 0;
	audio_player_buffering = false;
	audio_player_running = false;

	sysclk_enable_peripheral_clock(ID_TC0);
    tc_init(TC0, 0, TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC);
	tc_enable_interrupt(TC0, 0, TC_IER_CPCS);
}

void audio_player_start() {
	console_put_formatted("AudioPlayer> Starting, frequency: %i", AUDIO_PLAYER_SAMPLE_FREQUENCY);

	audio_player_running = true;

	float T_ms = 1000.0 / AUDIO_PLAYER_SAMPLE_FREQUENCY;
	// tc_write_rc(TC0, 0, (uint32_t)(T_ms * CLOCKS_PER_SEC * 32));
	tc_write_rc(TC0, 0, (uint32_t)(T_ms * CLOCKS_PER_SEC * 25));

	NVIC_EnableIRQ(TC0_IRQn);
    tc_start(TC0, 0);
}

void audio_player_stop() {
	console_put_formatted("AudioPlayer> Stopping");

	audio_player_running = false;

	NVIC_DisableIRQ(TC0_IRQn);
	tc_stop(TC0, 0);
}

void TC0_Handler() {
	if (audio_player_buffered_samples > 0) {
		uint8_t sample_value = 0;
		fifo_read(audio_player_buffer, &sample_value, 1);

		dac_write(sample_value);

		audio_player_buffered_samples--;
	}

	tc_get_status(TC0, 0);
}

void audio_player_ensure_buffered(void) {
	static int32_t last_refill = 0;
	
	int32_t current_time = timeguard_get_time_ms();

	if (audio_player_running && current_time - last_refill > 300 && audio_player_buffered_samples < 2000) {
		last_refill = current_time;

        audio_player_fill_buffer();
    }
}

void audio_player_fill_buffer() {
	console_put_formatted("AudioPlayer> Filling buffer...");

	esp_module_get_next_chunk();
}

__EXTERN_C_END
