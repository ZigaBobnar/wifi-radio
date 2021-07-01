#include "common.h"
#include "drivers/buttons.h"
#include "drivers/esp_module.h"
#include "drivers/dac.h"
#include "drivers/lcd.h"
#include "drivers/console.h"
#include "utils/timeguard.h"
#include "app/audio_player.h"
#include "app/ui.h"
#include "app/setup.h"
#include <time.h>

__EXTERN_C_BEGIN

bool system_setup(void);
void system_main_loop(void);

int main(void) {
    /* sets the processor clock according to conf_clock.h definitions */
    sysclk_init();

    /* disable wathcdog */
    WDT->WDT_MR = WDT_MR_WDDIS;

    /*****************************   HW init     *****************************/
    bool setup_successful = setup_begin();
    if (setup_successful) {
        // Initialization has succeeded.
        ui_set_state(UI_STATE_CLOCK);

        /***************************   Main loop   ***************************/
        while (1) {
            system_main_loop();
        }
    } else {
        ui_set_state(UI_STATE_ERROR);
        // Boot has failed, abort.
        int32_t delay_start_time = timeguard_get_time_ms();
        while (!timeguard_timeout_ms(delay_start_time, 3000)) {
            ui_run();
        }

        lcd_write_upper_formatted("Boot setup fail");
        lcd_write_lower_formatted("Cannot continue");

        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
                         "\n\nBoot setup has failed...\nCannot continue." CONSOLE_VT100_COLOR_TEXT_DEFAULT);

#if WIFI_RADIO_AUTOREBOOT == 1
        console_put_line(CONSOLE_VT100_COLOR_TEXT_RED
                         "Resetting in 10 seconds." CONSOLE_VT100_COLOR_TEXT_DEFAULT);

        delay_start_time = timeguard_get_time_ms();
        while (!timeguard_timeout_ms(delay_start_time, 10000)) {
            ui_run();
        }

        __DSB;
        SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
        RSTC->RSTC_CR = RSTC_CR_KEY(0xA5) | RSTC_CR_PERRST | RSTC_CR_PROCRST;
        NVIC_SystemReset();
#endif

        while (1) {
            ui_run();
        }
    }

    while (1) {}
}

void system_main_loop() {
    ui_run();
    audio_player_ensure_buffered();
}

__EXTERN_C_END
