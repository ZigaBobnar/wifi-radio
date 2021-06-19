#ifndef AUDIO_PLAYER_H_
#define AUDIO_PLAYER_H_

#include "common.h"
#include "fifo.h"

/**
 * Audio player implementation
 */

#ifndef AUDIO_PLAYER_SAMPLE_QUEUE_SIZE
#define AUDIO_PLAYER_SAMPLE_QUEUE_SIZE 512
#endif  // AUDIO_PLAYER_SAMPLE_QUEUE_SIZE

#ifndef AUDIO_PLAYER_SAMPLE_FREQUENCY
#define AUDIO_PLAYER_SAMPLE_FREQUENCY 16000
#endif  // AUDIO_PLAYER_SAMPLE_FREQUENCY

__EXTERN_C_BEGIN

extern uint8_t audio_sample_buff[AUDIO_PLAYER_SAMPLE_QUEUE_SIZE];
extern fifo_t audio_sample_fifo;

extern bool is_running;

void audio_player_init(void);
void audio_player_start(void);
void audio_player_stop(void);

__EXTERN_C_END

#endif  // AUDIO_PLAYER_H_
