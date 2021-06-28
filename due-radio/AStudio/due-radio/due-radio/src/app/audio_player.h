#ifndef AUDIO_PLAYER_H_
#define AUDIO_PLAYER_H_

#include "common.h"
#include "utils/fifo.h"

/**
 * Audio player implementation
 */

#ifndef AUDIO_PLAYER_SAMPLE_QUEUE_SIZE
#define AUDIO_PLAYER_SAMPLE_QUEUE_SIZE 17000
#endif  // AUDIO_PLAYER_SAMPLE_QUEUE_SIZE

#ifndef AUDIO_PLAYER_SAMPLE_FREQUENCY
#define AUDIO_PLAYER_SAMPLE_FREQUENCY 12000
#endif  // AUDIO_PLAYER_SAMPLE_FREQUENCY

#ifndef AUDIO_PLAYER_SAMPLE_REFILL_THRESHOLD
#define AUDIO_PLAYER_SAMPLE_REFILL_THRESHOLD 3000
#endif  // AUDIO_PLAYER_SAMPLE_REFILL_THRESHOLD

#ifndef AUDIO_PLAYER_SAMPLE_REFILL_TIME_THRESHOLD
#define AUDIO_PLAYER_SAMPLE_REFILL_TIME_THRESHOLD 200
#endif  // AUDIO_PLAYER_SAMPLE_REFILL_TIME_THRESHOLD

__EXTERN_C_BEGIN

// extern uint8_t audio_sample_buff[AUDIO_PLAYER_SAMPLE_QUEUE_SIZE];
// extern fifo_t audio_sample_fifo;

fifo_t* audio_player_buffer;
extern int32_t audio_player_buffered_samples;
extern bool audio_player_buffering;
extern int32_t audio_player_buffering_samples_left;
extern bool audio_player_running;

void audio_player_init(void);
void audio_player_start(void);
void audio_player_stop(void);
void audio_player_ensure_buffered(void);
void audio_player_fill_buffer(void);

__EXTERN_C_END

#endif  // AUDIO_PLAYER_H_
