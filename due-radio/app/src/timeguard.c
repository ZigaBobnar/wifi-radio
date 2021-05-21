#include "timeguard.h"
#include <time.h>

__EXTERN_C_BEGIN

const int32_t time_unit_ms = CLOCKS_PER_SEC * 32;
const int32_t time_unit_s = CLOCKS_PER_SEC * 32 * 1000;


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

__EXTERN_C_END
