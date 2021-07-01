#ifndef DRIVERS_CLOCK_H_
#define DRIVERS_CLOCK_H_

#include "common.h"

/**
 * Valid ranges for hardware RTC:
 * Century: 19, 20
 * Year BCD
 * Date: 01 - 31
 * Month: BCD, 01 - 12
 * Day: 1 - 7
 * Hour: 0 - 23 and not AM/PM flag or 01 - 12 and AM/PM flag
 * Minute: BCD, 00 - 59
 * Seconds: BCD, 00 - 59
 */

__EXTERN_C_BEGIN

void clock_init(void);
// void clock_set_date(uint8_t century, uint16_t year, uint8_t month, uint8_t date);
void clock_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
// void clock_get_date(uint16_t* year, uint8_t* month, uint8_t* day);
void clock_get_time(uint8_t* hours, uint8_t* minutes, uint8_t* seconds);
void clock_update_from_rtc(void);

__EXTERN_C_END

#endif  // DRIVERS_CLOCK_H_
