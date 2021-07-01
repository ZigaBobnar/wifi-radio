#ifndef UTILS_TIMEGUARD_H_
#define UTILS_TIMEGUARD_H_

#include "common.h"

/**
 * TimeGuard
 * 
 * This serves as a simple semi preconfigured way to track the time used by
 * anything so it can be decided if has been running for too long and stopped.
 * 
 * It uses hardware timer counter TC0, channel 1 as default.
 */

#ifndef TIMEGUARD_CUSTOM_CONFIG
#define TIMEGUARD_TCn TC0
#define TIMEGUARD_CHANNEL (1)
#define TIMEGUARD_ID_TCx ID_TC1
#endif  // TIMEGUARD_CUSTOM_CONFIG

__EXTERN_C_BEGIN

extern const int32_t time_unit_ms;
extern const int32_t time_unit_s;

/**
 * Initializes TimeGuard with default settings.
 */
void timeguard_init(void);

/**
 * Returns the raw time as it is obtained from timer counter.
 * This value needs to be properly scaled.
 */
int32_t timeguard_get_time(void);

/**
 * Returns value of timer counter scaled into milliseconds.
 */
int32_t timeguard_get_time_ms(void);

/**
 * Returns the difference between current value of timer counter and provided
 * value in milliseconds.
 */
int32_t timeguard_get_diff_ms(int32_t previous_time_ms);

/**
 * Returns value of timer counter scaled into seconds.
 */
int32_t timeguard_get_time_s(void);

/**
 * Returns the difference between current value of timer counter and provided
 * value in seconds.
 */
int32_t timeguard_get_diff_s(int32_t previous_time_s);

/**
 * Checks whether the timeout in milliseconds has been reached.
 */
bool timeguard_timeout_ms(int32_t start_time_ms, int32_t timeout_ms);

__EXTERN_C_END

#endif  // UTILS_TIMEGUARD_H_
