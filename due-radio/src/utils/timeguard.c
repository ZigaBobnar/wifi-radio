#include "utils/timeguard.h"
#include <time.h>

__EXTERN_C_BEGIN

// For some reason the multiplier is not 32 but somewhere between 24 and 25
const int32_t time_unit_ms = CLOCKS_PER_SEC * 25;
const int32_t time_unit_s = CLOCKS_PER_SEC * 25 * 1000;


void timeguard_init() {
    sysclk_enable_peripheral_clock(TIMEGUARD_ID_TCx);
    tc_init(TIMEGUARD_TCn, TIMEGUARD_CHANNEL, TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_WAVE);
    tc_start(TIMEGUARD_TCn, TIMEGUARD_CHANNEL);
}

int32_t timeguard_get_time() {
    return tc_read_cv(TIMEGUARD_TCn, TIMEGUARD_CHANNEL);
}

int32_t timeguard_get_time_ms() {
    return (timeguard_get_time() / time_unit_ms);
}

int32_t timeguard_get_diff_ms(int32_t previous_time_ms) {
    return timeguard_get_time_ms() - previous_time_ms;
}

int32_t timeguard_get_time_s() {
    return (timeguard_get_time() / time_unit_s);
}

int32_t timeguard_get_diff_s(int32_t previous_time_s) {
    return timeguard_get_time_s() - previous_time_s;
}

bool timeguard_timeout_ms(int32_t start_time_ms, int32_t timeout_ms) {
    return (timeguard_get_time_ms() - start_time_ms) >= timeout_ms;
}

__EXTERN_C_END
