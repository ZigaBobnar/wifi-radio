#ifndef APP_AUDIO_PLAYER_H_
#define APP_AUDIO_PLAYER_H_

#include "due-radio/common.h"
#include "due-radio/utils/fifo.h"

/**
 * Audio player implementation
 */

#ifndef AUDIO_PLAYER_SAMPLE_BUFFER_SIZE
#define AUDIO_PLAYER_SAMPLE_BUFFER_SIZE 15000
#endif  // AUDIO_PLAYER_SAMPLE_BUFFER_SIZE

#ifndef AUDIO_PLAYER_SAMPLE_FREQUENCY
#define AUDIO_PLAYER_SAMPLE_FREQUENCY 12000
#endif  // AUDIO_PLAYER_SAMPLE_FREQUENCY

#ifndef AUDIO_PLAYER_REBUFFER_MAX_SAMPLES_THRESHOLD
#define AUDIO_PLAYER_REBUFFER_MAX_SAMPLES_THRESHOLD 3000
#endif  // AUDIO_PLAYER_REBUFFER_MAX_SAMPLES_THRESHOLD

#ifndef AUDIO_PLAYER_MIN_REBUFFER_TIME_THRESHOLD
#define AUDIO_PLAYER_MIN_REBUFFER_TIME_THRESHOLD 300
#endif  // AUDIO_PLAYER_MIN_REBUFFER_TIME_THRESHOLD

__EXTERN_C_BEGIN

void audio_player_init(void);
void audio_player_start(void);
void audio_player_stop(void);
void audio_player_ensure_buffered(void);
void audio_player_fill_buffer(void);

__EXTERN_C_END

#endif  // APP_AUDIO_PLAYER_H_
