#include "due-radio/drivers/clock.h"
#include "due-radio/app/runtime.h"
#include "due-radio/utils/timeguard.h"

void clock_init() {
#if REAL_HARDWARE
    // Initialize to 24-hour mode
    RTC->RTC_MR &= ~RTC_MR_HRMOD;

    RTC->RTC_IER |= RTC_IER_SECEN;
    NVIC_EnableIRQ(RTC_IRQn);
#endif
}

void clock_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
#if REAL_HARDWARE
    rtc_set_time(RTC, hours, minutes, seconds);
#else
    (void)hours;
    (void)minutes;
    (void)seconds;
#endif
}

/*void clock_get_date(uint16_t* year, uint8_t* month, uint8_t* day) {
    if (runtime->clock->clock_dirty) {
        clock_update_from_rtc();
    }

    *year = runtime->clock->year;
    *month = runtime->clock->month;
    *day = runtime->clock->day;
}*/

void clock_get_time(uint8_t* hours, uint8_t* minutes, uint8_t* seconds) {
#if REAL_HARDWARE
    if (runtime->clock->clock_dirty) {
        clock_update_from_rtc();
    }
#else
    clock_update_from_rtc();
#endif

    *hours = runtime->clock->hour;
    *minutes = runtime->clock->minute;
    *seconds = runtime->clock->second;
}

void clock_update_from_rtc() {
    uint32_t hour, minute, second;

#if REAL_HARDWARE
    //uint32_t year, month, day, week;
    rtc_get_time(RTC, &hour, &minute, &second);
    //rtc_get_date(RTC, &year, &month, &day, &week);
#else
    hour = runtime->clock->hour;
    minute = runtime->clock->minute;
    second = runtime->clock->second;

    static uint32_t last_update_time_ms;
    uint32_t new_update_time_ms = timeguard_get_time_ms();
    uint32_t time_diff_ms = new_update_time_ms - last_update_time_ms;

    uint32_t hour_diff, minute_diff, second_diff;
    hour_diff = time_diff_ms / (1000 * 60 * 60);
    time_diff_ms -= hour_diff * 1000 * 60 * 60;
    minute_diff = time_diff_ms / (1000 * 60);
    time_diff_ms -= minute_diff * 1000 * 60;
    second_diff = time_diff_ms / 1000;
    time_diff_ms -= second_diff * 1000;

    second += second_diff;
    if (second >= 60) {
        uint8_t minute_extra = second / 60;
        minute_diff += minute_extra;
        second -= minute_extra * 60;
    }
    minute += minute_diff;
    if (minute >= 60) {
        uint8_t hour_extra = minute / 60;
        hour_diff += hour_extra;
        minute -= hour_extra * 60;
    }
    hour += hour_diff;
    if (hour >= 24) {
        uint8_t day_extra = hour / 24;
        hour -= day_extra * 24;
    }

    last_update_time_ms = new_update_time_ms - time_diff_ms;
#endif

    runtime->clock->hour = (uint8_t)hour;
    runtime->clock->minute = (uint8_t)minute;
    runtime->clock->second = (uint8_t)second;

    runtime->clock->clock_dirty = false;
}

void RTC_Handler() {
#if REAL_HARDWARE
    if (RTC->RTC_IMR & RTC_IMR_SEC) {
        runtime->clock->clock_dirty = true;
    }
#endif
}
