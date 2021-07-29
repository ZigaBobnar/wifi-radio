#include "drivers/clock.h"
#include "app/runtime.h"
#include <rtc.h>

void clock_init() {
    // Initialize to 24-hour mode
    RTC->RTC_MR &= ~RTC_MR_HRMOD;

    RTC->RTC_IER |= RTC_IER_SECEN;
    NVIC_EnableIRQ(RTC_IRQn);
}

void clock_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    rtc_set_time(RTC, hours, minutes, seconds);
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
    if (runtime->clock->clock_dirty) {
        clock_update_from_rtc();
    }

    *hours = runtime->clock->hour;
    *minutes = runtime->clock->minute;
    *seconds = runtime->clock->second;
}

void clock_update_from_rtc() {
    uint32_t hour, minute, second;
    //uint32_t year, month, day, week;
    rtc_get_time(RTC, &hour, &minute, &second);
    //rtc_get_date(RTC, &year, &month, &day, &week);

    runtime->clock->hour = (uint8_t)hour;
    runtime->clock->minute = (uint8_t)minute;
    runtime->clock->second = (uint8_t)second;

    runtime->clock->clock_dirty = false;
}

void RTC_Handler() {
    if (RTC->RTC_IMR & RTC_IMR_SEC) {
        runtime->clock->clock_dirty = true;
    }
}
